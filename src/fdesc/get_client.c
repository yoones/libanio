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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "libanio.h"

int		libanio_get_client(t_anio *server, int fd, t_fdesc **fdesc)
{
  t_lnode	*w;

  if (x_pthread_mutex_lock(&server->clients_mutex) != 0)
    return (-1);
  for (w = server->clients.head; w != NULL; w = w->next)
    {
      *fdesc = w->data;
      if ((*fdesc)->fd == fd)
	{
	  (void)x_pthread_mutex_unlock(&server->clients_mutex);
	  return (0);
	}
    }
  (void)x_pthread_mutex_unlock(&server->clients_mutex);
  return (-1);
}
