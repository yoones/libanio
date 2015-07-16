#include <pthread.h>
#include <stdio.h>
#include <string.h>

int		x_pthread_mutex_lock(pthread_mutex_t *mutex)
{
  int		ret;

  ret = pthread_mutex_lock(mutex);
  if (ret != 0)
    dprintf(2, "%s\n", strerror(ret));
  return (ret);
}

int		x_pthread_mutex_trylock(pthread_mutex_t *mutex)
{
  int		ret;

  ret = pthread_mutex_trylock(mutex);
  if (ret != 0)
    dprintf(2, "%s\n", strerror(ret));
  return (ret);
}

int		x_pthread_mutex_unlock(pthread_mutex_t *mutex)
{
  int		ret;

  ret = pthread_mutex_unlock(mutex);
  if (ret != 0)
    dprintf(2, "%s\n", strerror(ret));
  return (ret);
}
