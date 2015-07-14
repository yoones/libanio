#include "libanio.h"

int		libanio_set_callback_on_accept(t_anio *server,
					       t_anio_fptr_on_accept fptr_on_accept)
{
  server->fptr_on_accept = fptr_on_accept;
  return (0);
}
