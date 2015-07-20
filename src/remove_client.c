#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "libanio.h"

/*
  todo: rewrite code to have afront api that:
  - locks the right mutex,
  - calls a hidden (static?) functions that do the real work,
  - unlocks the mutex
 */
static int	_get_client(t_anio *server, int fd, t_fdesc **fdesc)
{
  t_lnode	*w;

  for (w = server->clients.head; w != NULL; w = w->next)
    {
      *fdesc = w->data;
      if ((*fdesc)->fd == fd)
	return (0);
    }
  return (-1);
}

int		libanio_remove_client(t_anio *server, int fd)
{
  t_fdesc	*fdesc;
  int		ret;

  if (x_pthread_mutex_lock(&server->clients_mutex) != 0)
    return (-1);
  ret = 0;
  if (_get_client(server, fd, &fdesc) != 0)
    {
      ret = -1;
      goto clear_and_leave;
    }
  if (epoll_ctl(server->thread_pool.epoll_fd, EPOLL_CTL_DEL, fdesc->fd, &fdesc->event) == -1)
    {
      print_err(errno);
      ret = -1;
    }
  libanio_fdesc_close(fdesc);
  free(fdesc);
  list_pop_data(&server->clients, fdesc);
  ret = 0;
 clear_and_leave:
  ret |= x_pthread_mutex_unlock(&server->clients_mutex);
  return (ret);
}
