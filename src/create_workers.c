#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "libanio.h"

/* int		libanio_fdesc_init(t_fdesc *fdesc, int fd); */
/* int		libanio_fdesc_close(t_fdesc *fdesc); */

/* int		libanio_has_client(t_anio *server, int fd); */
/* int		libanio_add_client(t_anio *server, int fd); */
/* int		libanio_remove_client(t_anio *server, int fd); */
/* int		libanio_get_client(t_anio *server, int fd, t_fdesc **fdesc); */

static int		_handle_error(t_anio *server, int fd, int errnumber)
{
  dprintf(2, "Error: epoll/error on file descriptor %d (errnumber: %d)\n", fd, errnumber);
  if (server->fptr_on_error)
    server->fptr_on_error(server, fd, 0); /* todo: find the right error number, using 0 meanwhile... */
  if (fd == server->fdesc.fd)
    {
      dprintf(2, "TODO: handle error on server's fd.\n");
      if (epoll_ctl(server->thread_pool.epoll_fd, EPOLL_CTL_DEL, server->fdesc.fd, &server->fdesc.event) == -1)
	perror("epoll_ctl(server)");
      /* todo: notify the monitor that it has to stop, closing all clients connexions and destroying all workers */
    }
  else
    libanio_remove_client(server, fd);
  return (0);
}

static int		_handle_read(t_anio *server, int fd)
{
  char			buff[4096];
  int			ret;

  ret = read(fd, buff, 4096);
  server->fptr_on_read(server, fd, NULL, ret); /* or call on_eof() ?? */
  /* if syscall fails, call fptr_on_error */
  return (-1);
}

static int		_handle_write(t_anio *server, int fd)
{
  (void)server;
  dprintf(2, "DEBUG: handle write on fd %d\n", fd);
  return (-1);
}

static int		_handle_accept(t_anio *server)
{
  int			client_fd;
  struct sockaddr_in	client_sin;
  socklen_t		client_addrlen;

  /* printf("DEBUG: accepting new client...\n"); */
  client_addrlen = sizeof(struct sockaddr_in);
  client_fd = accept(server->fdesc.fd, (struct sockaddr *)&client_sin, &client_addrlen);
  if (client_fd == -1)
    {
      perror(NULL);
      close(server->fdesc.fd);
      return (-1);
   }
  if (libanio_add_client(server, client_fd) != 0)
    {
      close(client_fd);
      return (-1);
    }
  if (server->fptr_on_accept)
    server->fptr_on_accept(server, client_fd);
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
      /* printf("DEBUG: worker mutex_lock\n"); */
      /* ret = pthread_mutex_lock(&server->thread_pool.jobs_mutex); */
      /* BREAK_ON_ERR(ret); */
      printf("DEBUG: worker cond_wait\n");
      ret = pthread_cond_wait(&server->thread_pool.jobs_condvar, &server->thread_pool.jobs_mutex);
      BREAK_ON_ERR(ret);
      printf("DEBUG: worker checking remaining jobs (%d)\n", server->thread_pool.remaining_jobs);
      if (server->thread_pool.remaining_jobs == 0)
	{
	  printf("DEBUG: worker found no job, mutex_unlock\n");
	  ret = pthread_mutex_unlock(&server->thread_pool.jobs_mutex);
	  BREAK_ON_ERR(ret);
	  continue ;
	}
      i = --server->thread_pool.remaining_jobs;
      printf("DEBUG: worker took job %d, mutex_unlock\n", i);
      ret = pthread_mutex_unlock(&server->thread_pool.jobs_mutex);
      BREAK_ON_ERR(ret);
#undef BREAK_ON_ERR
      if ((jobs[i].events & EPOLLERR) ||
	  (jobs[i].events & EPOLLHUP) ||
	  (!(jobs[i].events & EPOLLIN)))
	{
	  _handle_error(server, jobs[i].data.fd, 0);
	  continue ;
	}
      else if (jobs[i].data.fd == server->fdesc.fd)
	{
	  if (_handle_accept(server) != 0)
	    {
	      /*  handle error */
	    }
	}
      else if (server->fptr_on_read)
	{
	  _handle_read(server, jobs[i].data.fd);
	  /* client ready for read and/or write */
	}
    }
  printf("DEBUG: worker exits\n");
  pthread_exit((void *)EXIT_FAILURE);
  return (0);
}

static void	_list_free_worker(void *data)
{
  pthread_t	*worker = data;

  pthread_cancel(*worker);
  free(worker);
}

int		libanio_create_workers(t_anio *server)
{
  size_t	i;
  pthread_t	*worker;
  int		ret;

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
