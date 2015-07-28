/*
  This file is part of libanio (https://github.com/yoones/libanio).

  libanio is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  libanio is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with libanio.  If not, see <http://www.gnu.org/licenses/>
*/

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
