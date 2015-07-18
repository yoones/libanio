#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "libanio.h"

int		libanio_add_client(t_anio *server, int fd)
{
  t_fdesc	*fdesc;
  int		ret;

  if (libanio_has_client(server, fd) == 1)
    return (-1);
  if (!(fdesc = malloc(sizeof(t_fdesc)))
      || libanio_fdesc_init(fdesc, fd) == -1)
    {
      perror(NULL);
      free(fdesc);
      return (-1);
    }
  fdesc->event.data.fd = fdesc->fd;
  fdesc->event.events = EPOLLIN; /* todo: watch event EPOLLRDHUP to detected closed socket */
  if (fdesc->writebuf.size > 0)
    fdesc->event.events |= EPOLLOUT;
  if (x_epoll_ctl(server->thread_pool.epoll_fd, EPOLL_CTL_ADD, fdesc->fd, &fdesc->event) != 0
      || x_pthread_mutex_lock(&server->clients_mutex) != 0)
    {
      free(fdesc);
      return (-1);
    }
  ret = 0;
  if (list_push_front(&server->clients, fdesc) == -1)
    {
      free(fdesc);
      ret = -1;
    }
  ret |= x_pthread_mutex_unlock(&server->clients_mutex);
  return (ret);
}
