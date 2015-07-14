#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include "libanio.h"

/* int epoll_create(int size); */

#define EPOLL_MAX_EVENTS 1024
/* todo: if getrlimit(NOFILE) > 0, use this value instead of EPOLL_MAX_EVENTS  */

static int	_fill_fds_pool(t_anio *server,
			       int epoll_fd,
			       struct epoll_event *events_pool)
{
  int		i;
  t_lnode	*w;
  t_anio_fd	*aniofd;

  /* add server */
  events_pool[0].data.fd = server->fd;
  events_pool[0].events = EPOLLIN | EPOLLIN; /* todo: use EPOLLRDHUP to detected closed socket */
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
      events_pool[i].events = EPOLLIN | EPOLLIN; /* todo: use EPOLLRDHUP to detected closed socket */
      if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, aniofd->fd, events_pool + i) == -1)
	{
	  perror(NULL);
	  return (-1);
	}
    }
  return (0);
}

static int		_read_events(t_anio *server,
				     int epoll_fd,
				     struct epoll_event *events)
{
  int			n;
  int			i;

  while (1)
    {
      if ((n = epoll_wait(epoll_fd, events, EPOLL_MAX_EVENTS, -1)) == -1)
	{
	  perror(NULL);
	  return (-1);
	}
      for (i = 0; i < n; i++)
	{
	  if ((events[i].events & EPOLLERR) ||
	      (events[i].events & EPOLLHUP) ||
	      (!(events[i].events & EPOLLIN)))
	    {
	      /* An error has occured on this fd, or the socket is not
		 ready for reading (why were we notified then?) */
	      dprintf(2, "epoll error\n"); /* todo: get the real error to display a useful message here */
	      close(events[i].data.fd);
	      /* call on_error(), then remove this fd from clients' list */
	      /* if this fd is the server's, stop the monitor and set server->fd to -1 */
	      continue ;
	    }
	  else if (events[i].data.fd == server->fd)
	    {
	      /* accept new client if limit not reached */
	    }
	  else
	    {
	      /* client ready for read and/or write */
	    }
	}
    }
  return (0);
}

static void		*_start_monitor(void *arg)
{
  t_anio		*server = arg;
  int			epoll_fd;
  int			nb_fds, prev_nb_fds;
  struct epoll_event	*fds_pool = NULL;
  struct epoll_event	*events;

  (void)server;
  if ((epoll_fd = epoll_create1(0)) == -1)
    {
      perror(NULL);
      pthread_exit((void *)EXIT_FAILURE);
    }
  if (!(events = calloc(EPOLL_MAX_EVENTS, sizeof(struct epoll_event))))
    {
      perror(NULL);
      close(epoll_fd);
      pthread_exit((void *)EXIT_FAILURE);
    }
  prev_nb_fds = 0;
  while (1)
    {
      nb_fds = ((int)server->max_clients < server->clients.size) ? server->clients.size : (int)server->max_clients; /* todo: use only size_t here to avoid risky casts */
      nb_fds++;		/* one more slot for the server's fd */
      if (nb_fds != prev_nb_fds)
	{
	  fds_pool = realloc(fds_pool, sizeof(struct epoll_event) * nb_fds);
	  if (!fds_pool)
	    {
	      perror(NULL);
	      close(epoll_fd);
	      pthread_exit((void *)EXIT_FAILURE);
	    }
	}
      prev_nb_fds = nb_fds;
      if (_fill_fds_pool(server, epoll_fd, fds_pool) == -1)
	{
	  close(epoll_fd);
	  free(fds_pool);
	  pthread_exit((void *)EXIT_FAILURE);
	}
      /* read events here */
      _read_events(server, epoll_fd, events);
    }
  pause();			/* debug */
  free(fds_pool);
  pthread_exit((void *)EXIT_SUCCESS);
  return (NULL);
}

int		libanio_start_monitor(t_anio *server)
{
  int		ret;

  if (pthread_mutex_trylock(&server->monitoring_thread_mutex) != 0)
    return (-1);
  if ((ret = pthread_create(&server->monitoring_thread,
			    &server->monitoring_thread_attr,
			    &_start_monitor,
			    (void *)server)) != 0)
    {
      dprintf(2, "%s\n", strerror(ret));
      return (-1);
    }
  return (0);
}
