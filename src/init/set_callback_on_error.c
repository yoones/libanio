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
