#include "libanio.h"

static void	_do_nothing(t_anio *server, int fd, char *buf, size_t size)
{
  (void)server;
  (void)fd;
  (void)buf;
  (void)size;
}

int		libanio_set_callback_on_eof(t_anio *server,
					    t_anio_fptr_on_eof fptr_on_eof)
{
  server->fptr_on_eof = (fptr_on_eof ? fptr_on_eof : &_do_nothing);
  return (0);
}
