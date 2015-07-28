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

#ifndef LIBANIO_INTERNAL_INTERFACE_H_
# define LIBANIO_INTERNAL_INTERFACE_H_

# include "libanio_types.h"

int		libanio_start_monitor(t_anio *server);
int		libanio_stop_monitor(t_anio *server);

int		libanio_create_workers(t_anio *server);
int		libanio_destroy_workers(t_anio *server);
int		libanio_handle_event(t_anio *server, struct epoll_event *job);

int		libanio_fdesc_init(t_fdesc *fdesc, int fd);
int		libanio_fdesc_close(t_fdesc *fdesc);

int		libanio_has_client(t_anio *server, int fd);
int		libanio_add_client(t_anio *server, int fd);
int		libanio_remove_client(t_anio *server, int fd);
int		libanio_get_client(t_anio *server, int fd, t_fdesc **fdesc);

/* pthread helpers */
int		x_pthread_mutex_lock(pthread_mutex_t *mutex);
int		x_pthread_mutex_trylock(pthread_mutex_t *mutex);
int		x_pthread_mutex_unlock(pthread_mutex_t *mutex);
int		x_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
				 void *(*start_routine) (void *), void *arg);
int		x_pthread_cancel(pthread_t thread);
int		x_pthread_cond_broadcast(pthread_cond_t *cond);
int		x_pthread_cond_wait(pthread_cond_t *cond,
				    pthread_mutex_t *mutex);

/* epoll helpers */

int		x_epoll_create1(int flags);
int		x_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int		x_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

#endif /* LIBANIO_INTERNAL_INTERFACE_H_ */
