#include <stdio.h>
#include <string.h>
#include "libanio.h"

int		libanio_stop_monitor(t_anio *server)
{
  int		ret;

  if (pthread_mutex_trylock(&server->monitoring_thread_mutex) == 0)
    {
      pthread_mutex_unlock(&server->monitoring_thread_mutex);
      return (-1);
    }
  libanio_destroy_workers(server);
  list_clear(&server->clients);
  if ((ret = pthread_cancel(server->monitoring_thread)) != 0)
    {
      print_err(ret);
      return (-1);
    }
  pthread_mutex_unlock(&server->monitoring_thread_mutex);
  return (0);
}
