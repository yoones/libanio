#include "libanio.h"

int		libanio_destroy_workers(t_anio *server)
{
  list_clear(&server->thread_pool.workers);
  return (0);
}
