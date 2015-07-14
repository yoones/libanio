#include "libanio.h"

int		libanio_set_max_clients(t_anio *server,
					size_t nb)
{
  server->max_clients = nb;
  return (0);
}
