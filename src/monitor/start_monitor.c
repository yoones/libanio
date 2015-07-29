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
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "libanio.h"

#define EPOLL_MAX_EVENTS 1024
/* todo: if getrlimit(NOFILE) > 0, use this value instead of EPOLL_MAX_EVENTS  */

static int	_watch_server_fd(t_anio *server)
{
  server->fdesc.event.data.fd = server->fdesc.fd;
  server->fdesc.event.events = EPOLLIN; /* todo: use EPOLLRDHUP to detected closed socket */
  if (epoll_ctl(server->thread_pool.epoll_fd, EPOLL_CTL_ADD, server->fdesc.fd, &server->fdesc.event) == -1)
    {
      print_err(errno);
      return (-1);
    }
  return (0);
}

static int		_wake_up_workers(t_anio *server)
{
  if (server->thread_pool.remaining_jobs > 0)
    {
      if (server->thread_pool.workers.size == 0)
	{
	  print_custom_err("ERROR: no worker available!!!");
	  abort();	/* todo: manage this case. should the monitor run if there's no worker?? */
	}
      if (x_pthread_cond_broadcast(&server->thread_pool.jobs_condvar))
	return (-1);
      return (EAGAIN);
    }
  else if (server->thread_pool.busy_workers > 0)
    return (EAGAIN);
  else
    return (0);
}

static int		_let_workers_consume_events(t_anio *server)
{
  int			ret;

  do
    {
      if (x_pthread_mutex_lock(&server->thread_pool.jobs_mutex))
	return (-1);
      ret = _wake_up_workers(server);
      if (x_pthread_mutex_unlock(&server->thread_pool.jobs_mutex))
	return (-1);
    } while (ret == EAGAIN);
  return (EAGAIN);
}

static int		_monitor_loop(t_anio *server)
{
  int			ret;

  do
    {
      if (x_pthread_mutex_lock(&server->thread_pool.jobs_mutex))
	return (-1);
      if ((server->thread_pool.remaining_jobs = x_epoll_wait(server->thread_pool.epoll_fd,
							     server->thread_pool.jobs,
							     EPOLL_MAX_EVENTS, -1)) == -1)
	{
	  ret = -1;
	  server->thread_pool.remaining_jobs = 0;
	  x_pthread_mutex_unlock(&server->thread_pool.jobs_mutex);
	  return (-1);
	}
      if (x_pthread_mutex_unlock(&server->thread_pool.jobs_mutex))
	return (-1);
      ret = _let_workers_consume_events(server);
    } while (ret == EAGAIN);
  return (0);
}

static void		*_monitor_main(void *arg)
{
  t_anio		*server = arg;
  int			ret;

  if ((server->thread_pool.epoll_fd = epoll_create1(0)) == -1
      || !(server->thread_pool.jobs = calloc(EPOLL_MAX_EVENTS, sizeof(struct epoll_event)))
      || libanio_create_workers(server) == -1)
    {
      print_err(errno);
      close(server->thread_pool.epoll_fd);
      server->thread_pool.epoll_fd = -1;
      pthread_exit((void *)EXIT_FAILURE);
    }
  if (_watch_server_fd(server) == -1)
    {
      print_custom_err("ERROR: CANNOT WATCH SERVER FD");
      close(server->thread_pool.epoll_fd);
      server->thread_pool.epoll_fd = -1;
      libanio_destroy_workers(server);
      pthread_exit((void *)EXIT_FAILURE);
    }
  ret = _monitor_loop(server);
  (void)libanio_destroy_workers(server);
  close(server->thread_pool.epoll_fd);
  server->thread_pool.epoll_fd = -1;
  free(server->thread_pool.jobs);
  server->thread_pool.jobs = NULL;
  server->thread_pool.remaining_jobs = 0;
  (void)x_pthread_mutex_unlock(&server->thread_pool.jobs_mutex);
  pthread_exit(ret == 0 ? (void *)EXIT_SUCCESS : (void *)EXIT_FAILURE);
  return (NULL);
}

int		libanio_start_monitor(t_anio *server)
{
  if (x_pthread_mutex_trylock(&server->monitoring_thread_mutex) != 0) /* could use a pthread_tryjoin instead? */
    return (-1);
  if (x_pthread_create(&server->monitoring_thread, NULL, &_monitor_main, (void *)server) != 0)
    return (-1);
  return (0);
}
