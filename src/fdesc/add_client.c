/*
  This file is part of libanio (https://github.com/yoones/libanio).

  libanio is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  libanio is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with libanio.  If not, see <http://www.gnu.org/licenses/>
*/

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
      print_err(errno);
      free(fdesc);
      return (-1);
    }
  fdesc->event.data.fd = fdesc->fd;
  fdesc->event.events = EPOLLIN;
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
