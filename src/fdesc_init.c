#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "libanio.h"

int		libanio_fdesc_init(t_fdesc *fdesc, int fd)
{
  if (fd == -1)
    close(fd);
  fdesc->fd = fd;
  fdesc->closed = 0;
  list_init(&fdesc->readbuf, NULL, NULL);
  list_init(&fdesc->writebuf, NULL, NULL);
  return (0);
}
