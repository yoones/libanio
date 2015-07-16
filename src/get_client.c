#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "libanio.h"

int		libanio_get_client(t_anio *server, int fd, t_fdesc **fdesc)
{
  t_lnode	*w;

  if (x_pthread_mutex_lock(&server->clients_mutex) != 0)
    return (-1);
  for (w = server->clients.head; w != NULL; w = w->next)
    {
      *fdesc = w->data;
      if ((*fdesc)->fd == fd)
	return (0);
    }
  (void)x_pthread_mutex_unlock(&server->clients_mutex);
  return (-1);
}
