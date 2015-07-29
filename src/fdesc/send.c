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

#include <stdlib.h>
#include <stdio.h>
#include "libanio.h"

int		libanio_send(t_anio *server, int fd, char *buf, size_t nbytes)
{
  t_fdesc	*fdesc;
  t_anio_buf	*anio_buf;

  if (libanio_get_client(server, fd, &fdesc))
    return (-1);
  printf("DEBUG: send(%d), %d packets awaiting!\n", nbytes, fdesc->writebuf.size);
  if (!(anio_buf = malloc(sizeof(t_anio_buf)))
      || !(anio_buf->data = malloc(sizeof(char) * nbytes)))
    {
      print_err(errno);
      free(anio_buf->data);
      free(anio_buf);
      return (-1);
    }
  memcpy(anio_buf->data, buf, nbytes);
  anio_buf->size = nbytes;
  if (list_push_back(&fdesc->writebuf, (void *)anio_buf))
    {
      print_err(errno);
      free(anio_buf->data);
      free(anio_buf);
      return (-1);
    }
  if (x_epoll_ctl(server->thread_pool.epoll_fd, EPOLL_CTL_DEL, fdesc->fd, &fdesc->event) != 0)
    return (-1);
  fdesc->event.events = EPOLLIN | EPOLLOUT;
  if (x_epoll_ctl(server->thread_pool.epoll_fd, EPOLL_CTL_ADD, fdesc->fd, &fdesc->event) != 0)
    return (-1);
  return (0);
}

int		libanio_send_string(t_anio *server, int fd, char *buf)
{
  return (libanio_send(server, fd, buf, strlen(buf)));
}
