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

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "libanio.h"

int		x_pthread_mutex_lock(pthread_mutex_t *mutex)
{
  int		ret;

  ret = pthread_mutex_lock(mutex);
  if (ret != 0)
    print_err(ret);
  return (ret);
}

int		x_pthread_mutex_trylock(pthread_mutex_t *mutex)
{
  int		ret;

  ret = pthread_mutex_trylock(mutex);
  if (ret != 0)
    print_err(ret);
  return (ret);
}

int		x_pthread_mutex_unlock(pthread_mutex_t *mutex)
{
  int		ret;

  ret = pthread_mutex_unlock(mutex);
  if (ret != 0)
    print_err(ret);
  return (ret);
}

int		x_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
				 void *(*start_routine) (void *), void *arg)
{
  int		ret;

  ret = pthread_create(thread, attr, start_routine, arg);
  if (ret != 0)
    print_err(ret);
  return (ret);
}

int		x_pthread_cancel(pthread_t thread)
{
  int		ret;

  ret = pthread_cancel(thread);
  if (ret != 0)
    print_err(ret);
  return (ret);
}

int		x_pthread_cond_broadcast(pthread_cond_t *cond)
{
  int		ret;

  ret = pthread_cond_broadcast(cond);
  if (ret != 0)
    print_err(ret);
  return (ret);
}

int		x_pthread_cond_wait(pthread_cond_t *cond,
				    pthread_mutex_t *mutex)
{
  int		ret;

  ret = pthread_cond_wait(cond, mutex);
  if (ret != 0)
    print_err(ret);
  return (ret);
}
