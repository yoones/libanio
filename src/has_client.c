#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "libanio.h"

int		libanio_has_client(t_anio *server, int fd)
{
  t_fdesc	*tmp;

  return (libanio_get_client(server, fd, &tmp) == 0 ? 1 : 0);
}
