#include <stdlib.h>
#include "libanio.h"

/* int pthread_attr_destroy(pthread_attr_t *attr); */
/* int pthread_mutex_destroy(pthread_mutex_t *mutex); */

void		libanio_free(t_anio *server)
{
  (void)server;
  abort();			/* todo: write list:free callbacks first */
}
