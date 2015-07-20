#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "libanio.h"

static void	_list_free_client(void *data)
{
  t_fdesc	*fdesc = data;

  libanio_fdesc_close(fdesc);
  free(fdesc);
}

static void	_list_free_worker(void *data)
{
  pthread_t	*thread = data;

  pthread_cancel(*thread);
  pthread_join(*thread, NULL);
  free(thread);
}

int		libanio_init(t_anio *server,
			     int fd,
			     size_t max_clients,
			     size_t thread_pool_size,
			     t_anio_fptr_on_accept fptr_on_accept,
			     t_anio_fptr_on_read fptr_on_read,
			     t_anio_fptr_on_eof fptr_on_eof,
			     t_anio_fptr_on_error fptr_on_error,
			     t_anio_mode mode,
			     ...)
{
  va_list	ap;

  server->max_clients = max_clients;
  list_init(&server->clients, &_list_free_client, NULL);
  pthread_mutex_init(&server->clients_mutex, NULL);
  if (thread_pool_size == 0)
    return (-1);
  server->thread_pool.max_workers = thread_pool_size;
  list_init(&server->thread_pool.workers, &_list_free_worker, NULL);
  server->thread_pool.epoll_fd = -1;
  server->thread_pool.jobs = NULL;
  server->thread_pool.busy_workers = 0;
  server->thread_pool.remaining_jobs = 0;
  pthread_mutex_init(&server->thread_pool.jobs_mutex, NULL);
  pthread_cond_init(&server->thread_pool.jobs_condvar, NULL);
  if (libanio_set_callbacks(server, fptr_on_accept, fptr_on_read, fptr_on_eof, fptr_on_error) == -1)
    return (-1);
  server->mode = mode;
  va_start(ap, mode);
  switch (mode)
    {
    case (ANIO_MODE_STREAM):
      break ;
    case (ANIO_MODE_BLOCK):
      server->mode_config.block_size = va_arg(ap, size_t);
      if (server->mode_config.block_size == 0)
	return (-1);
      break ;
    case (ANIO_MODE_LINE):
      server->mode_config.line_delim = va_arg(ap, char *);
      if (server->mode_config.line_delim == NULL)
	return (-1);
      break ;
    default:
      return (-1);
    }
  va_end(ap);
  pthread_mutex_init(&server->monitoring_thread_mutex, NULL);
  /*
    server fd != -1 means the server is alive, so I set it at the end of this function,
    so libanio_is_server_alive() can check it to know if the server is running
    (meaning libanio_init() succeeded and no syscall on server->fd failed)
   */
  libanio_fdesc_init(&server->fdesc, fd);
  return (0);
}
