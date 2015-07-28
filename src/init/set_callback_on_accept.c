#include "libanio.h"

static void	_do_nothing(t_anio *server, int fd)
{
  (void)server;
  (void)fd;
}

int		libanio_set_callback_on_accept(t_anio *server,
					       t_anio_fptr_on_accept fptr_on_accept)
{
  server->fptr_on_accept = (fptr_on_accept ? fptr_on_accept : &_do_nothing);
  return (0);
}
