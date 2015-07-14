#include "libanio.h"

int		libanio_set_callback_on_error(t_anio *server,
					      t_anio_fptr_on_error fptr_on_error)
{
  server->fptr_on_error = fptr_on_error;
  return (0);
}
