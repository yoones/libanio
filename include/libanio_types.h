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

#ifndef LIBANIO_TYPES_H_
# define LIBANIO_TYPES_H_

/**
 * @file
 */

/** @struct s_anio_buf
 *  @brief This structure holds data that will be used later for i/o (when the related file descriptor is ready for it)
 *  @var s_anio_buf::data
 *  Member 'data' contains the bytes read or to write
 *  @var s_anio_buf::size
 *  Member 'size' contains the number of bytes available in data
 */
typedef struct	s_anio_buf
{
  char		*data;
  size_t	size;
}		t_anio_buf;

typedef enum	e_anio_mode
  {
    ANIO_MODE_STREAM,
    ANIO_MODE_BLOCK,
    ANIO_MODE_LINE
  }		t_anio_mode;

/** @union u_anio_mode_config
 *  @brief This union holds precisions depending on the reading mode set.
 *  @var u_anio_mode_config::line_delim
 *  Member 'line_delim' contains the end-of-line string delimiter.
 *  @var u_anio_mode_config::block_size
 *  Member 'block_size' contains the size of a block.
 */
typedef union	u_anio_mode_config
{
  char		*line_delim;
  size_t	block_size;
}		t_anio_mode_config;

/** @struct s_fdesc
 *  @brief This structure represents a client.
 *  @var s_fdesc::fd
 *  Member 'fd' contains the client's file descriptor.
 *  @var s_fdesc::closed
 *  Member 'closed' tells if the file descriptors has been closed.
 *  @var s_fdesc::event
 *  Member 'event' contains the event read by the monitor using epoll for a worker to handle it.
 *  @var s_fdesc::readbuf
 *  Member 'readbuf' contains the bytes received that are waiting to be consumed.
 *  @var s_fdesc::writebuf
 *  Member 'writebuf' contains the bytes to be sent.
 */
typedef struct		s_fdesc
{
  int			fd;
  int			closed;
  struct epoll_event	event;
  t_list		readbuf;
  t_list		writebuf;
}			t_fdesc;

struct s_anio;

typedef void (*t_anio_fptr_on_accept)(struct s_anio *, int);
typedef void (*t_anio_fptr_on_read)(struct s_anio *, int, char *, size_t);
typedef void (*t_anio_fptr_on_eof)(struct s_anio *, int, char *, size_t);
typedef void (*t_anio_fptr_on_error)(struct s_anio *, int, int);

/** @struct s_thread_pool
 *  @brief This structure holds data used to manage the workers and the events they shall handle.
 *  @var s_thread_pool::max_workers
 *  Member 'max_workers' contains the maximum number of threads wanted.
 *  @var s_thread_pool::workers
 *  Member 'workers' is a linked list of workers.
 *  @var s_thread_pool::epoll_fd
 *  Member 'epoll_fd' contains epoll's file descriptor used by the monitor to wait for events.
 *  @var s_thread_pool::jobs
 *  Member 'jobs' is an array used where the monitor stores events for the workers to read them.
 *  @var s_thread_pool::remaining_jobs
 *  Member 'remaining_jobs' contains the remaining number of jobs that workers have to take care of.
 *  @var s_thread_pool::busy_workers
 *  Member 'busy_workers' contains the current number of workers currently handling events.
 *  @var s_thread_pool::jobs_mutex
 *  Member 'jobs_mutex' is a lock to safely access the jobs from different threads.
 *  @var s_thread_pool::jobs_condvar
 *  Member 'jobs_condvar' is a conditional variable used to wake up workers when events are to be handled.
 */
typedef struct		s_thread_pool
{
  size_t		max_workers;
  t_list		workers;

  int			epoll_fd;
  struct epoll_event	*jobs;
  int			remaining_jobs;
  int			busy_workers;
  pthread_mutex_t	jobs_mutex;
  pthread_cond_t	jobs_condvar;
}			t_thread_pool;

/** @struct s_anio
 *  @brief This structure holds the necessary elements for libanio to manage the server's fd.
 *  @var s_anio::fdesc
 *  Member 'fdesc' represents the server's file descriptor.
 *  @var s_anio::max_clients
 *  Member 'max_clients' is the maximum number of clients the server should manage simultaneously.
 *  @var s_anio::clients
 *  Member 'clients' is a linked list of clients.
 *  @var s_anio::clients_mutex
 *  Member 'clients_mutex' is a lock to safely access the clients list from different threads.
 *  @var s_anio::monitoring_thread_mutex
 *  Member 'remaining_jobs' is a lock to make sure this server's monitor is only run once at a time.
 *  @var s_anio::monitoring_thread
 *  Member 'monitoring_thread' contains the monitor thread id.
 *  @var s_anio::thread_pool
 *  Member 'thread_pool' represents this server's thread pool.
 *  @var s_anio::fptr_on_accept
 *  Member 'fptr_on_accept' is a function pointer on a handler to be called back when a new client is accepted.
 *  @var s_anio::fptr_on_read
 *  Member 'fptr_on_read' is a function pointer on a handler to be called back when data is ready to be consumed (see mode).
 *  @var s_anio::fptr_on_eof
 *  Member 'fptr_on_eof' is a function pointer on a handler to be called back when a client disconnects.
 *  @var s_anio::fptr_on_error
 *  Member 'fptr_on_error' is a function pointer on a handler to be called back when an error occurs.
 *  @var s_anio::mode
 *  Member 'mode' tells what reading mode to use (ANIO_MODE_STREAM: consum data as it comes, ANIO_MODE_BLOCK: wait until a block of prefixed size is complete before consuming it, ANIO_MODE_LINE: wait until a line with prefixed end-of-line delimiter is complete before consuming it).
 *  @var s_anio::mode_config
 *  Member 'mode_config' sets, depending on the mode, either the block size or the end-of-line delimiter.
 */
typedef struct		s_anio
{
  t_fdesc		fdesc;

  size_t		max_clients;
  t_list		clients;
  pthread_mutex_t	clients_mutex;

  pthread_mutex_t	monitoring_thread_mutex;
  pthread_t		monitoring_thread;

  t_thread_pool		thread_pool;

  t_anio_fptr_on_accept	fptr_on_accept;
  t_anio_fptr_on_read	fptr_on_read;
  t_anio_fptr_on_eof	fptr_on_eof;
  t_anio_fptr_on_error	fptr_on_error;

  t_anio_mode		mode;
  t_anio_mode_config	mode_config;
}			t_anio;

#endif
