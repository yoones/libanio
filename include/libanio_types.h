#ifndef LIBANIO_TYPES_H_
# define LIBANIO_TYPES_H_

/**
 * @file
 */

typedef struct	s_anio_buf
{
  char		*data;
  size_t	size;
  size_t	memsize;
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

typedef struct	s_anio_fd
{
  int		fd;
  /* t_list	readbuf; */
  /* t_list	writebuf; */
}		t_anio_fd;

struct s_anio;

typedef void (*t_anio_fptr_on_accept)(struct s_anio *, t_anio_fd *);
typedef void (*t_anio_fptr_on_read)(struct s_anio *, t_anio_fd *, char *, size_t);
typedef void (*t_anio_fptr_on_eof)(struct s_anio *, t_anio_fd *, char *, size_t);
typedef void (*t_anio_fptr_on_error)(struct s_anio *, t_anio_fd *, int);

typedef struct		s_anio
{
  int			fd;

  pthread_mutex_t	monitoring_thread_mutex;
  pthread_attr_t	monitoring_thread_attr;
  pthread_t		monitoring_thread;

  size_t		max_clients;
  t_list		clients;

  size_t		thread_pool_size;
  t_list		threads;

  t_anio_fptr_on_accept	fptr_on_accept;
  t_anio_fptr_on_read	fptr_on_read;
  t_anio_fptr_on_eof	fptr_on_eof;
  t_anio_fptr_on_error	fptr_on_error;

  t_anio_mode		mode;
  t_anio_mode_config	mode_config;
}			t_anio;

#endif
