#ifndef LIBANIO_INTERNAL_INTERFACE_H_
# define LIBANIO_INTERNAL_INTERFACE_H_

/*
  Needed:
  - one thread for epoll
  - thread pool for i/o and callbacks
 */

# include "libanio_types.h"

int		libanio_start_monitor(t_anio *server);
int		libanio_stop_monitor(t_anio *server);

#endif /* LIBANIO_INTERNAL_INTERFACE_H_ */
