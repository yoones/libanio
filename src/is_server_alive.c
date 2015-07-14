#include "libanio.h"

int		libanio_is_server_alive(t_anio *server)
{
  return (server->fd != -1 ? 0 : -1);
}
