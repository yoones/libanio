#include "libanio.h"

int		libanio_set_callback_on_read(t_anio *server,
					     t_anio_fptr_on_read fptr_on_read)
{
  if (fptr_on_read == NULL)
    return (-1);
  server->fptr_on_read = fptr_on_read;
  return (0);
}
