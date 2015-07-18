#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "libanio.h"

int		libanio_remove_client(t_anio *server, int fd)
{
  t_lnode	*w;
  t_fdesc	*fdesc;
  int		ret;

  if (x_pthread_mutex_lock(&server->clients_mutex) != 0)
    return (-1);
#define DO_NOT_EXIT 42
  ret = DO_NOT_EXIT;
  do
    {
      if (server->clients.size == 0)
	{
	  ret = -1;
	  break ;
	}
      fdesc = server->clients.head->data;
      if (fdesc->fd == fd)
	{
	  if (epoll_ctl(server->thread_pool.epoll_fd, EPOLL_CTL_DEL, fdesc->fd, &fdesc->event) == -1)
	    perror("epoll_ctl(client)");
	  libanio_fdesc_close(server->clients.head->data);
	  list_pop_front(&server->clients);
	  ret = 0;
	  break ;
	}
      fdesc = server->clients.tail->data;
      if (fdesc->fd == fd)
	{
	  if (epoll_ctl(server->thread_pool.epoll_fd, EPOLL_CTL_DEL, fdesc->fd, &fdesc->event) == -1)
	    perror("epoll_ctl(client)");
	  libanio_fdesc_close(server->clients.tail->data);
	  list_pop_back(&server->clients);
	  ret = 0;
	  break ;
	}
      if (server->clients.size < 3)
	{
	  ret = -1;
	  break ;
	}
    } while (0);
  if (ret != DO_NOT_EXIT)
    {
      /* mutex unlock */
      if (x_pthread_mutex_unlock(&server->clients_mutex) != 0)
	return (-1);
      return (ret);
    }
#undef DO_NOT_EXIT
  for (w = server->clients.head->next; w != NULL; w = w->next)
    {
      fdesc = w->data;
      if (fdesc->fd == fd)
	{
	  w->next->prev = w->prev;
	  w->prev->next = w->next;
	  server->clients.size--;
	  if (epoll_ctl(server->thread_pool.epoll_fd, EPOLL_CTL_DEL, fdesc->fd, &fdesc->event) == -1)
	    perror("epoll_ctl(client)");
	  libanio_fdesc_close(w->data);
	  free(w->data);
	  free(w);
	  if (x_pthread_mutex_unlock(&server->clients_mutex) != 0)
	    return (-1);
	  return (0);
	}
    }
  if (x_pthread_mutex_unlock(&server->clients_mutex) != 0)
    return (-1);
  return (-1);
}
