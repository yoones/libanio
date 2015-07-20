#include <stdlib.h>
#include "libanio.h"

void		libanio_free(t_anio *server)
{
  server->fdesc.fd = -1;
  libanio_fdesc_close(&server->fdesc);
  pthread_mutex_destroy(&server->clients_mutex);
  pthread_mutex_destroy(&server->thread_pool.jobs_mutex);
  pthread_cond_destroy(&server->thread_pool.jobs_condvar);
}
