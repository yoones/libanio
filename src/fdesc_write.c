#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include "list.h"
#include "fdesc.h"

int		fdesc_write(t_fdesc *fdesc, char *buf, size_t size)
{
  t_fdesc_buf	*fdesc_buf;

  if (!(fdesc_buf = malloc(sizeof(t_fdesc_buf))))
    {
      fdesc->errno_cache = errno;
      return (-1);
    }
  if (!(fdesc_buf->data = malloc(sizeof(char) * size)))
    {
      fdesc->errno_cache = errno;
      free(fdesc_buf);
      return (-1);
    }
  memcpy(fdesc_buf->data, buf, size);
  fdesc_buf->size = size;
  if (list_push_back(&fdesc->writebuf, fdesc_buf))
    {
      fdesc->errno_cache = errno;
      free(fdesc_buf->data);
      free(fdesc_buf);
      return (-1);
    }
  fdesc->errno_cache = 0;
  return (0);
}
