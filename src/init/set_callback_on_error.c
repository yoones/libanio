#include "libanio.h"

static void	_do_nothing(t_anio *server, int fd, int errnumber)
{
  (void)server;
  (void)fd;
  (void)errnumber;
}

int		libanio_set_callback_on_error(t_anio *server,
					      t_anio_fptr_on_error fptr_on_error)
{
  server->fptr_on_error = (fptr_on_error ? fptr_on_error : &_do_nothing);
  return (0);
}
