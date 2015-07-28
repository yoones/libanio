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
#include <string.h>
#include "libanio.h"

int		libanio_fdesc_init(t_fdesc *fdesc, int fd)
{
  if (fd == -1)
    close(fd);
  fdesc->fd = fd;
  fdesc->closed = 0;
  memset(&fdesc->event, 0, sizeof(struct epoll_event));
  list_init(&fdesc->readbuf, NULL, NULL);
  list_init(&fdesc->writebuf, NULL, NULL);
  return (0);
}
