#ifndef LIBANIO_TYPES_H_
# define LIBANIO_TYPES_H_

/**
 * @file
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

typedef union	u_anio_mode_config
{
  char		*line_delim;
  size_t	block_size;
}		t_anio_mode_config;

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
