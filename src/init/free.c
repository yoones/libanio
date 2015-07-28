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


#include <stdlib.h>
#include "libanio.h"

void		libanio_free(t_anio *server)
{
  server->fdesc.fd = -1;
  libanio_fdesc_close(&server->fdesc);
  pthread_mutex_destroy(&server->clients_mutex);
  pthread_mutex_destroy(&server->thread_pool.jobs_mutex);
  pthread_cond_destroy(&server->thread_pool.jobs_condvar);
}
