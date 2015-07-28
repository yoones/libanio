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

#include <stdio.h>
#include <string.h>
#include "libanio.h"

int		libanio_stop_monitor(t_anio *server)
{
  int		ret;

  if (pthread_mutex_trylock(&server->monitoring_thread_mutex) == 0)
    {
      pthread_mutex_unlock(&server->monitoring_thread_mutex);
      return (-1);
    }
  libanio_destroy_workers(server);
  list_clear(&server->clients);
  if ((ret = pthread_cancel(server->monitoring_thread)) != 0)
    {
      print_err(ret);
      return (-1);
    }
  pthread_mutex_unlock(&server->monitoring_thread_mutex);
  return (0);
}
