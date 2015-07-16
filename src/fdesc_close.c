#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "libanio.h"

int		libanio_fdesc_close(t_fdesc *fdesc)
{
  if (fdesc->fd == -1)
    return (-1);
  close(fdesc->fd);
  fdesc->closed = 1;
  list_clear(&fdesc->readbuf);
  list_clear(&fdesc->writebuf);
  return (0);
}
