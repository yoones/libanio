#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include "list.h"
#include "fdesc.h"

static int	_fdesc_buf_cmp(void *d1, void *d2)
{
  t_fdesc_buf	*dest = d1;
  t_fdesc_buf	*src = d2;

  return (strcmp(dest->data, src->data));
}

static void	_fdesc_buf_free(void *d)
{
  t_fdesc_buf	*fdesc_buf = d;

  free(fdesc_buf->data);
  free(fdesc_buf);
}

t_fdesc		*fdesc_alloc(int fd, char *eol)
{
  t_fdesc	*fdesc;

  if (!(fdesc = malloc(sizeof(t_fdesc))))
    return (NULL);
  if (fdesc_init(fdesc, fd, eol) == -1)
    {
      free(fdesc);
      return (NULL);
    }
  return (fdesc);
}

int		fdesc_init(t_fdesc *fdesc, int fd, char *eol)
{
  fdesc->fd = fd;
  fdesc->errno_cache = 0;
  if (eol)
    {
      if (!(fdesc->eol = strdup(eol)))
	return (-1);
    }
  else
    fdesc->eol = NULL;
  fdesc->eof = 0;
  list_init(&fdesc->readbuf, &_fdesc_buf_free, &_fdesc_buf_cmp);
  list_init(&fdesc->writebuf, &_fdesc_buf_free, &_fdesc_buf_cmp);
  list_init(&fdesc->lines_available, (t_list_data_free *)&free, (t_list_data_cmp *)&strcmp);
  return (0);
}

void		fdesc_free(t_fdesc *fdesc)
{
  free(fdesc->eol);
  list_clear(&fdesc->readbuf);
  list_clear(&fdesc->writebuf);
  list_clear(&fdesc->lines_available);
}
