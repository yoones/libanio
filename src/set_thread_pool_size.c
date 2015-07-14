#include "libanio.h"

int		libanio_set_thread_pool_size(t_anio *server,
					     size_t size)
{
  if (size == 0)
    return (-1);
  server->thread_pool_size = size;
  return (0);
}
