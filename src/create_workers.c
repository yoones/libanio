#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "libanio.h"

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
      else if (jobs[i].data.fd == server->fdesc.fd)
	{
	  printf("debug: accepting new client...\n");
	  int			client_fd;
	  struct sockaddr_in	client_sin;
	  socklen_t		client_addrlen;

	  /* accept new client if limit not reached */
	  client_addrlen = sizeof(struct sockaddr_in);
	  client_fd = accept(server->fdesc.fd, (struct sockaddr *)&client_sin, &client_addrlen);
	  if (client_fd == -1)
	    {
	      perror("");
	      /* on_error server */
	      close(server->fdesc.fd);
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
