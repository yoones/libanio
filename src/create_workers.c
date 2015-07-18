#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "libanio.h"

static int		_handle_error(t_anio *server, int fd, int errnumber)
{
  server->fptr_on_error(server, fd, 0); /* todo: find the right error number, using 0 meanwhile... */
  libanio_remove_client(server, fd);
  return (0);
}

static int		_handle_read(t_anio *server, int fd)
{
  char			buff[4096];
  int			ret;

  ret = read(fd, buff, 4096);
  switch (ret)
    {
    case (-1):
      ret = errno;
      perror(NULL);
      return (_handle_error(server, fd, ret));
    case (0):
      /* todo: extract unread data, send it to on_eof callback */
      server->fptr_on_eof(server, fd, NULL, 0); /* buf NULL size 0 for debug */
      libanio_remove_client(server, fd);
      break ;
    default:
      /* stack data or call callback depending on the read mode (stream, block or line) */
      server->fptr_on_read(server, fd, NULL, ret); /* debug: work in stream mode for now */
    }
  return (0);
}

static int		_handle_write(t_anio *server, int fd)
{
  DEBUG(GREEN, "DEBUG: worker -> _handle_write");
  (void)server;
  dprintf(2, "TODO: handle write on fd %d\n", fd);
  return (-1);
}

static int		_handle_accept(t_anio *server)
{
  int			client_fd;
  struct sockaddr_in	client_sin;
  socklen_t		client_addrlen;

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
  server->fptr_on_accept(server, client_fd);
  return (0);
}

static int		_handle_event(t_anio *server, struct epoll_event *job)
{
  if (job->data.fd == server->fdesc.fd)
    {
      if ((job->events & EPOLLERR)
	  || (job->events & EPOLLHUP)
	  || (!(job->events & EPOLLIN)))
	{
	  dprintf(2, "TODO: handle error on server's fd.\n");
	  /* todo: notify the monitor that it has to stop, closing all clients connexions and destroying all workers */
	  return (-1);
	}
      else if (job->events & EPOLLIN)
	return (_handle_accept(server));
    }
  else
    {
      if ((job->events & EPOLLERR)
	  || (job->events & EPOLLHUP)
	  || (!(job->events & EPOLLIN)))
	return (_handle_error(server, job->data.fd, 0));
      else if (job->events & EPOLLIN)
	return (_handle_read(server, job->data.fd));
      else if (job->events & EPOLLOUT)
	return (_handle_write(server, job->data.fd));
    }
  DEBUG(GREEN, "DEBUG: worker -> no handler set for this case");
  return (-1);
}

static void		*_worker_main(void *arg)
{
  t_anio		*server = arg;
  struct epoll_event	*jobs = server->thread_pool.jobs;
  struct epoll_event	my_job;	/* todo: move this into worker structure */

  while (1)
    {
      if (x_pthread_cond_wait(&server->thread_pool.jobs_condvar, &server->thread_pool.jobs_mutex))
	break ;
      if (server->thread_pool.remaining_jobs == 0)
	{
	  if (x_pthread_mutex_unlock(&server->thread_pool.jobs_mutex))
	    break ;
	  continue ;
	}
      server->thread_pool.remaining_jobs--;
      memcpy(&my_job, jobs + server->thread_pool.remaining_jobs, sizeof(struct epoll_event));
      if (x_pthread_mutex_unlock(&server->thread_pool.jobs_mutex))
	break ;
      if (_handle_event(server, &my_job) == -1)
	{ /* handle handler's error */ }
    }
  list_pop_data(&server->thread_pool.workers, (void *)pthread_self());
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
				&_worker_main,
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
