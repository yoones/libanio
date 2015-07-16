#include "libanio.h"

int		libanio_is_server_alive(t_anio *server)
{
  return (server->fdesc.fd != -1 ? 0 : -1);
}
