#include "libanio.h"

int		libanio_destroy_workers(t_anio *server)
{
  list_clear(&server->thread_pool.workers);
  x_pthread_cond_broadcast(&server->thread_pool.jobs_condvar);
  return (0);
}
