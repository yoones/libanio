#include "libanio.h"

int		libanio_destroy_workers(t_anio *server)
{
  server->thread_pool.max_workers = 0;
  x_pthread_cond_broadcast(&server->thread_pool.jobs_condvar);
  list_clear(&server->thread_pool.workers);
  return (0);
}
