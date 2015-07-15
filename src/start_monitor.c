#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "libanio.h"

/* int epoll_create(int size); */

#define EPOLL_MAX_EVENTS 1024
/* todo: if getrlimit(NOFILE) > 0, use this value instead of EPOLL_MAX_EVENTS  */

static int		_realloc_fds_pool(t_anio *server,
					  struct epoll_event **fds_pool,
					  int *prev_nb_fds)
{
  int			nb_fds;

  nb_fds = ((int)server->max_clients < server->clients.size) ? server->clients.size : (int)server->max_clients; /* todo: use only size_t here to avoid unsafe casts */
  nb_fds++;		/* one more slot for the server's fd */
  if (nb_fds != *prev_nb_fds)
    {
      *fds_pool = realloc(*fds_pool, sizeof(struct epoll_event) * nb_fds);
      if (!*fds_pool)
	{
	  perror(NULL);
	  return (-1);
	}
    }
  *prev_nb_fds = nb_fds;
  return (0);
}

static int	_fill_fds_pool(t_anio *server,
			       int epoll_fd,
			       struct epoll_event *events_pool)
{
  int		i;
  t_lnode	*w;
  t_anio_fd	*aniofd;

  /* add server */
  events_pool[0].data.fd = server->fd;
  events_pool[0].events = EPOLLIN; /* todo: use EPOLLRDHUP to detected closed socket */
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server->fd, events_pool + 0) == -1)
    {
      perror(NULL);
      return (-1);
    }
  /* add clients */
  for (i = 1, w = server->clients.head; w != NULL; w = w->next, i++)
    {
      aniofd = w->data;
      events_pool[i].data.fd = aniofd->fd;
      events_pool[i].events = EPOLLIN; /* todo: watch event EPOLLRDHUP to detected closed socket */
      if (aniofd->writebuf.size > 0)
	events_pool[i].events |= EPOLLOUT;
      if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, aniofd->fd, events_pool + i) == -1)
	{
	  perror(NULL);
	  return (-1);
	}
    }
  return (0);
}

static void		*_start_worker(void *arg)
{
  t_anio		*server = arg;
  struct epoll_event	*jobs = server->thread_pool.jobs;
  int			i;
  int			ret;

  while (1)
    {
#define BREAK_ON_ERR(ret) if (ret != 0) { dprintf(2, "%s\n", strerror(ret)); break ; }
      ret = pthread_mutex_lock(&server->thread_pool.jobs_mutex);
      BREAK_ON_ERR(ret);
      ret = pthread_cond_wait(&server->thread_pool.jobs_condvar, &server->thread_pool.jobs_mutex);
      BREAK_ON_ERR(ret);
      if (server->thread_pool.remaining_jobs == 0)
	{
	  ret = pthread_mutex_unlock(&server->thread_pool.jobs_mutex);
	  BREAK_ON_ERR(ret);
	  continue ;
	}
      i = --server->thread_pool.remaining_jobs;
      ret = pthread_mutex_unlock(&server->thread_pool.jobs_mutex);
      BREAK_ON_ERR(ret);
#undef BREAK_ON_ERR
      if ((jobs[i].events & EPOLLERR) ||
	  (jobs[i].events & EPOLLHUP) ||
	  (!(jobs[i].events & EPOLLIN)))
	{
	  /* An error has occured on this fd, or the socket is not
	     ready for reading (why were we notified then?) */
	  dprintf(2, "epoll error\n"); /* todo: get the real error to display a useful message here */
	  if (server->fptr_on_error)
	    server->fptr_on_error(server, jobs[i].data.fd, 0); /* todo: find the right error number, using 0 meanwhile... */
	  /* if this fd is the server's, stop the monitor and set server->fd to -1 */
	  close(jobs[i].data.fd);
	  /* remove this fd from clients' list */
	  continue ;
	}
      else if (jobs[i].data.fd == server->fd)
	{
	  printf("debug: accepting new client...\n");
	  int			client_fd;
	  struct sockaddr_in	client_sin;
	  socklen_t		client_addrlen;

	  /* accept new client if limit not reached */
	  client_addrlen = sizeof(struct sockaddr_in);
	  client_fd = accept(server->fd, (struct sockaddr *)&client_sin, &client_addrlen);
	  if (client_fd == -1)
	    {
	      perror("");
	      /* on_error server */
	      close(server->fd);
	      return ((void *)-1);
	    }
	  if (server->fptr_on_accept)
	    server->fptr_on_accept(server, client_fd);
	}
      else if (server->fptr_on_read)
	{
	  /* client ready for read and/or write */
	  server->fptr_on_read(server, jobs[i].data.fd, NULL, 0); /* or call on_eof() ?? */
	}
    }
  pthread_exit((void *)EXIT_FAILURE);
  return (0);
}

static int		_destroy_workers(t_anio *server)
{
  list_clear(&server->thread_pool.workers);
  return (0);
}

static void		_list_free_worker(void *data)
{
  pthread_t		*worker = data;

  pthread_cancel(*worker);
  free(worker);
}

static int		_create_workers(t_anio *server)
{
  size_t		i;
  pthread_t		*worker;
  int			ret;

  if (server->thread_pool.max_workers == 0
      || server->thread_pool.workers.size > 0)
    return (-1);
  list_init(&server->thread_pool.workers, &_list_free_worker, NULL);
  for (i = 0; i < server->thread_pool.max_workers; i++)
    {
      if (!(worker = malloc(sizeof(pthread_t))))
	{
	  perror(NULL);
	  list_clear(&server->thread_pool.workers);
	  return (-1);
	}
      if ((ret = pthread_create(worker,
				NULL,
				&_start_worker,
				(void *)server)) != 0)
	{
	  dprintf(2, "%s\n", strerror(ret));
	  free(worker);
	  list_clear(&server->thread_pool.workers);
	  return (-1);
	}
      if (list_push_back(&server->thread_pool.workers, worker) == -1)
	{
	  pthread_cancel(*worker);
	  free(worker);
	  list_clear(&server->thread_pool.workers);
	  return (-1);
	}
    }
  return (0);
}

static void		*_start_monitor(void *arg)
{
  t_anio		*server = arg;
  int			epoll_fd;
  int			prev_nb_fds;
  struct epoll_event	*fds_pool = NULL;
  int			err_flag;
  int			ret;

  if ((epoll_fd = epoll_create1(0)) == -1
      || !(server->thread_pool.jobs = calloc(EPOLL_MAX_EVENTS, sizeof(struct epoll_event)))
      || _create_workers(server) == -1)
    {
      perror(NULL);
      close(epoll_fd);
      pthread_exit((void *)EXIT_FAILURE);
    }
  prev_nb_fds = 0;
  err_flag = 0;
#define BREAK_ON_ERR(ret, err_flag) if (ret != 0) { dprintf(2, "%s\n", strerror(ret)); err_flag = 1; break ; }
  while (!err_flag)
    {
      /* prepare what epoll needs and wait for events */
      ret = pthread_mutex_lock(&server->thread_pool.jobs_mutex);
      BREAK_ON_ERR(ret, err_flag);
      if (_realloc_fds_pool(server, &fds_pool, &prev_nb_fds) == -1
	  || _fill_fds_pool(server, epoll_fd, fds_pool) == -1)
	break ;
      if ((server->thread_pool.remaining_jobs = epoll_wait(epoll_fd, server->thread_pool.jobs, EPOLL_MAX_EVENTS, -1)) == -1)
	{
	  server->thread_pool.remaining_jobs = 0;
	  perror(NULL);
	  break ;
	}
      ret = pthread_mutex_unlock(&server->thread_pool.jobs_mutex);
      BREAK_ON_ERR(ret, err_flag);
      /* let workers take care of epoll events */
      while (!err_flag)
	{
	  ret = pthread_mutex_lock(&server->thread_pool.jobs_mutex);
	  BREAK_ON_ERR(ret, err_flag);
	  if (server->thread_pool.remaining_jobs > 0)
	    {
	      ret = pthread_cond_broadcast(&server->thread_pool.jobs_condvar);
	      BREAK_ON_ERR(ret, err_flag);
	    }
	  ret = pthread_mutex_unlock(&server->thread_pool.jobs_mutex);
	  BREAK_ON_ERR(ret, err_flag);
	}
    }
#undef BREAK_ON_ERR
  (void) _destroy_workers(server);
  close(epoll_fd);
  free(fds_pool);
  free(server->thread_pool.jobs);
  server->thread_pool.jobs = NULL;
  server->thread_pool.remaining_jobs = 0;
  (void) pthread_mutex_unlock(&server->thread_pool.jobs_mutex);
  pthread_exit((void *)EXIT_FAILURE);
  return (NULL);
}

int		libanio_start_monitor(t_anio *server)
{
  int		ret;

  if (pthread_mutex_trylock(&server->monitoring_thread_mutex) != 0) /* could use a pthread_join instead? */
    return (-1);
  if ((ret = pthread_create(&server->monitoring_thread,
			    NULL,
			    &_start_monitor,
			    (void *)server)) != 0)
    {
      dprintf(2, "%s\n", strerror(ret));
      return (-1);
    }
  return (0);
}
