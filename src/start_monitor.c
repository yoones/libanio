#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "libanio.h"

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
  t_fdesc	*fdesc;

  /* add server */
  events_pool[0].data.fd = server->fdesc.fd;
  events_pool[0].events = EPOLLIN; /* todo: use EPOLLRDHUP to detected closed socket */
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server->fdesc.fd, events_pool + 0) == -1)
    {
      perror("epoll_ctl(server)");
      return (-1);
    }
  /* add clients */
  for (i = 1, w = server->clients.head; w != NULL; w = w->next, i++)
    {
      fdesc = w->data;
      events_pool[i].data.fd = fdesc->fd;
      events_pool[i].events = EPOLLIN; /* todo: watch event EPOLLRDHUP to detected closed socket */
      if (fdesc->writebuf.size > 0)
	events_pool[i].events |= EPOLLOUT;
      if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fdesc->fd, events_pool + i) == -1)
	{
	  perror("epoll_ctl(client)");
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
      || libanio_create_workers(server) == -1)
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
      printf("DEBUG: monitor mutex_lock (part 1) ??\n");
      ret = pthread_mutex_lock(&server->thread_pool.jobs_mutex);
      printf("DEBUG: monitor mutex_lock (part 1) OK (%d)\n", ret);
      BREAK_ON_ERR(ret, err_flag);
      if (_realloc_fds_pool(server, &fds_pool, &prev_nb_fds) == -1
	  || _fill_fds_pool(server, epoll_fd, fds_pool) == -1)
	{printf("FUUUUCK\n");break ;}
      printf("DEBUG: monitor epoll_wait\n");
      if ((server->thread_pool.remaining_jobs = epoll_wait(epoll_fd, server->thread_pool.jobs, EPOLL_MAX_EVENTS, -1)) == -1)
	{
	  server->thread_pool.remaining_jobs = 0;
	  perror(NULL);
	  break ;
	}
      printf("DEBUG: monitor mutex_unlock (part 1)\n");
      ret = pthread_mutex_unlock(&server->thread_pool.jobs_mutex);
      BREAK_ON_ERR(ret, err_flag);
      /* let workers take care of epoll events */
      while (!err_flag)
	{
	  printf("DEBUG: monitor mutex_lock (part 2)\n");
	  ret = pthread_mutex_lock(&server->thread_pool.jobs_mutex);
	  BREAK_ON_ERR(ret, err_flag);
	  printf("DEBUG: monitor has %d jobs\n", server->thread_pool.remaining_jobs);
	  if (server->thread_pool.remaining_jobs > 0)
	    {
	      printf("DEBUG: monitor cond_broadcast\n");
	      ret = pthread_cond_broadcast(&server->thread_pool.jobs_condvar);
	      BREAK_ON_ERR(ret, err_flag);
	    }
	  else
	    {
	      printf("DEBUG: monitor mutex_unlock\n");
	      ret = pthread_mutex_unlock(&server->thread_pool.jobs_mutex);
	      BREAK_ON_ERR(ret, err_flag);
	      break ;
	    }
	  printf("DEBUG: monitor mutex_unlock (part 2)\n");
	  ret = pthread_mutex_unlock(&server->thread_pool.jobs_mutex);
	  BREAK_ON_ERR(ret, err_flag);
	}
    }
#undef BREAK_ON_ERR
  (void)libanio_destroy_workers(server);
  close(epoll_fd);
  free(fds_pool);
  free(server->thread_pool.jobs);
  server->thread_pool.jobs = NULL;
  server->thread_pool.remaining_jobs = 0;
  (void)pthread_mutex_unlock(&server->thread_pool.jobs_mutex);
  pthread_exit((void *)EXIT_FAILURE);
  return (NULL);
}

int		libanio_start_monitor(t_anio *server)
{
  int		ret;

  if (pthread_mutex_trylock(&server->monitoring_thread_mutex) != 0) /* could use a pthread_tryjoin instead? */
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
