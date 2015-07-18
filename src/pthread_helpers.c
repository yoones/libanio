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

int		x_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
				 void *(*start_routine) (void *), void *arg)
{
  int		ret;

  ret = pthread_create(thread, attr, start_routine, arg);
  if (ret != 0)
    dprintf(2, "%s\n", strerror(ret));
  return (ret);
}

int		x_pthread_cancel(pthread_t thread)
{
  int		ret;

  ret = pthread_cancel(thread);
  if (ret != 0)
    dprintf(2, "%s\n", strerror(ret));
  return (ret);
}

int		x_pthread_cond_broadcast(pthread_cond_t *cond)
{
  int		ret;

  ret = pthread_cond_broadcast(cond);
  if (ret != 0)
    dprintf(2, "%s\n", strerror(ret));
  return (ret);
}

int		x_pthread_cond_wait(pthread_cond_t *cond,
				    pthread_mutex_t *mutex)
{
  int		ret;

  ret = pthread_cond_wait(cond, mutex);
  if (ret != 0)
    dprintf(2, "%s\n", strerror(ret));
  return (ret);
}
