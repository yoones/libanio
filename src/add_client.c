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
  if (!(fdesc = malloc(sizeof(t_fdesc))))
    {
      perror(NULL);
      return (-1);
    }
  if (libanio_fdesc_init(fdesc, fd) == -1
      || x_pthread_mutex_lock(&server->clients_mutex) != 0)
    {
      free(fdesc);
      return (-1);
    }
  ret = 0;
  if (list_push_front(&server->clients, fdesc) == -1)
    {
      free(fdesc);
      fdesc = NULL;
      ret = -1;
    }
  ret |= x_pthread_mutex_unlock(&server->clients_mutex);
  return (ret);
}
