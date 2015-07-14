#include "libanio.h"

int		libanio_set_callback_on_eof(t_anio *server,
					    t_anio_fptr_on_eof fptr_on_eof)
{
  server->fptr_on_eof = fptr_on_eof;
  return (0);
}
