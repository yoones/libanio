#ifndef LIBANIO_INTERNAL_INTERFACE_H_
# define LIBANIO_INTERNAL_INTERFACE_H_

/*
  Needed:
  - one thread for epoll
  - thread pool for i/o and callbacks
 */

# include "libanio_types.h"

int		x_pthread_mutex_lock(pthread_mutex_t *mutex);
int		x_pthread_mutex_trylock(pthread_mutex_t *mutex);
int		x_pthread_mutex_unlock(pthread_mutex_t *mutex);


int		libanio_start_monitor(t_anio *server);
int		libanio_stop_monitor(t_anio *server);

int		libanio_create_workers(t_anio *server);
int		libanio_destroy_workers(t_anio *server);

int		libanio_fdesc_init(t_fdesc *fdesc, int fd);
int		libanio_fdesc_close(t_fdesc *fdesc);

int		libanio_has_client(t_anio *server, int fd);
int		libanio_add_client(t_anio *server, int fd);
int		libanio_remove_client(t_anio *server, int fd);
int		libanio_get_client(t_anio *server, int fd, t_fdesc **fdesc);


#endif /* LIBANIO_INTERNAL_INTERFACE_H_ */
