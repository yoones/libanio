#include "libanio.h"

int		libanio_set_callbacks(t_anio *server,
				      t_anio_fptr_on_accept fptr_on_accept,
				      t_anio_fptr_on_read fptr_on_read,
				      t_anio_fptr_on_eof fptr_on_eof,
				      t_anio_fptr_on_error fptr_on_error)
{
  if (fptr_on_read == NULL)
    return (-1);
  libanio_set_callback_on_accept(server, fptr_on_accept);
  libanio_set_callback_on_read(server, fptr_on_read);
  libanio_set_callback_on_eof(server, fptr_on_eof);
  libanio_set_callback_on_error(server, fptr_on_error);
  return (0);
}
