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
#include <errno.h>
#include "libanio.h"

static size_t		_get_available_bytes(t_fdesc *fdesc)
{
  t_lnode		*w;
  t_anio_buf		*anio_buf;
  size_t		nbytes;

  nbytes = 0;
  for (w = fdesc->readbuf.head; w; w = w->next)
    {
      anio_buf = w->data;
      nbytes += anio_buf->size;
    }
  return (nbytes);
}

/* expects the requested 'size' bytes to be available */
static char		*_extract_bytes(t_fdesc *fdesc, size_t size, int extra_null_byte)
{
  char			*buf;
  char			*tmp;
  t_anio_buf		*anio_buf;
  size_t		nbytes;

  if (size == 0)
    return (strdup(""));
  extra_null_byte = (extra_null_byte ? 1 : 0);
  if (!(buf = malloc(sizeof(char) * (size + extra_null_byte))))
    {
      print_err(errno);
      return (NULL);
    }
  tmp = buf;
  do
    {
      anio_buf = fdesc->readbuf.head->data;
      nbytes = (size < anio_buf->size ? size : anio_buf->size);
      memcpy(tmp, anio_buf->data, nbytes);
      tmp += nbytes;
      if (size >= nbytes)
	{
	  free(anio_buf->data);
	  free(anio_buf);
	  list_pop_front(&fdesc->readbuf);
	}
      else
	{
	  memmove(anio_buf->data, anio_buf->data + nbytes, anio_buf->size - nbytes);
	  anio_buf->size -= nbytes;
	}
      size -= nbytes;
    } while (size > 0);
  if (extra_null_byte)
    *tmp = '\0';
  return (buf);
}

static int		_handle_error(t_anio *server, int fd, int errnumber)
{
  print_err(errnumber);
  server->fptr_on_error(server, fd, errnumber);
  libanio_remove_client(server, fd);
  return (0);
}

static int		_handle_eof(t_anio *server, int fd)
{
  char			*buf;
  size_t		size;
  t_fdesc		*fdesc;
  t_anio_buf		*anio_buf;
  t_lnode		*w;

  if (libanio_get_client(server, fd, &fdesc))
    {
      print_custom_err("Error: fd not found");
      return (-1);
    }
    size = 0;
    for (w = fdesc->readbuf.head; w; w = w->next)
      {
      anio_buf = (t_anio_buf *)w->data;
      size += anio_buf->size;
    }
  if ((buf = _extract_bytes(fdesc, size, (server->mode == ANIO_MODE_LINE ? 1 : 0))) == NULL)
    return (_handle_error(server, fd, errno));
  server->fptr_on_eof(server, fd, buf, size);
  return (libanio_remove_client(server, fd));
}

static int		_push_new_data(t_fdesc *fdesc, char *buf, size_t size)
{
  t_anio_buf		*anio_buf;

  if (!(anio_buf = malloc(sizeof(t_anio_buf)))
      || !(anio_buf->data = malloc(sizeof(char) * size)))
    {
      print_err(errno);
      free(anio_buf);
      return (-1);
    }
  memcpy(anio_buf->data, buf, size);
  anio_buf->size = size;
  if (list_push_back(&fdesc->readbuf, (void *)anio_buf))
    {
      print_err(errno);
      free(anio_buf->data);
      free(anio_buf);
      return (-1);
    }
  return (0);
}

static int		_get_line_size(t_anio *server, t_fdesc *fdesc, size_t *nbytes)
{
  int			delim_length;
  t_lnode		*w;
  t_anio_buf		*anio_buf;
  char			*tmp;

  *nbytes = 0;
  delim_length = strlen(server->mode_config.line_delim);
  for (w = fdesc->readbuf.head; w; w = w->next)
    {
      anio_buf = w->data;
      tmp = anio_buf->data;
      while (*tmp && strncmp(tmp, server->mode_config.line_delim, delim_length))
	{
	  (*nbytes)++;
	  tmp++;
	}
    }
  if (strncmp(tmp, server->mode_config.line_delim, delim_length))
    return (-1);
  *nbytes += delim_length;
  return (0);
}

static int		_handle_read(t_anio *server, int fd)
{
  char			buff[ANIO_BUF_SIZE];
  int			ret;
  char			*extract;
  t_fdesc		*fdesc;
  char			*tmp;
  size_t		nbytes;

  ret = read(fd, buff, ANIO_BUF_SIZE);
  if (ret == -1)
    return (_handle_error(server, fd, errno));
  else if (ret == 0)
    return (_handle_eof(server, fd));
  switch (server->mode)
    {
      /* === STREAM === */
    case (ANIO_MODE_STREAM):
      if (!(extract = malloc(sizeof(char) * ret)))
	return (_handle_error(server, fd, errno));
      memcpy(extract, buff, ret);
      server->fptr_on_read(server, fd, extract, ret);
      return (0);

      /* === BLOCK === */
    case (ANIO_MODE_BLOCK):
      if (libanio_get_client(server, fd, &fdesc))
	{
	  print_custom_err("Error: fd not found, should not happen!!");
	  return (-1);
	}
      if (_push_new_data(fdesc, buff, ret))
	return (_handle_error(server, fd, errno));
      if (_get_available_bytes(fdesc) < server->mode_config.block_size)
	return (0);
      if ((tmp = _extract_bytes(fdesc, server->mode_config.block_size, 0)) == NULL)
	return (_handle_error(server, fd, errno));
      server->fptr_on_read(server, fd, tmp, server->mode_config.block_size);
      return (0);

      /* === LINE === */
    case (ANIO_MODE_LINE):
      if (libanio_get_client(server, fd, &fdesc))
	{
	  print_custom_err("Error: fd not found, should not happen!!");
	  return (-1);
	}
      if (_push_new_data(fdesc, buff, ret))
	return (_handle_error(server, fd, errno));
      if (_get_line_size(server, fdesc, &nbytes) != 0)
	return (0);
      if ((tmp = _extract_bytes(fdesc, nbytes, 1)) == NULL)
	return (_handle_error(server, fd, errno));
      server->fptr_on_read(server, fd, tmp, nbytes);
      return (0);

    default:
      DEBUG(RED, "Error: unknown reading mode (%d), abort!!", server->mode);
      abort();
      return (-1);
    }
}

static int		_handle_write(t_anio *server, int fd)
{
  t_fdesc		*fdesc;
  t_anio_buf		*anio_buf;
  int			ret;

  if (libanio_get_client(server, fd, &fdesc))
    {
      print_custom_err("Error: fd not found, should not happen!!");
      return (-1);
    }
  anio_buf = fdesc->writebuf.head->data;
  ret = write(fd, anio_buf->data, anio_buf->size);
  if (ret == -1)
    return (_handle_error(server, fd, errno));
  else if (ret == 0)
    return (_handle_eof(server, fd));
  if (ret < (int)anio_buf->size)
    {
      memmove(anio_buf->data, anio_buf->data + ret, anio_buf->size - ret);
      anio_buf->size -= ret;
    }
  else
    {
      list_pop_front(&fdesc->writebuf);
      if (fdesc->writebuf.size == 0)
	{
	  if (x_epoll_ctl(server->thread_pool.epoll_fd, EPOLL_CTL_DEL, fdesc->fd, &fdesc->event) != 0)
	    return (-1);
	  fdesc->event.events = EPOLLIN;
	  if (x_epoll_ctl(server->thread_pool.epoll_fd, EPOLL_CTL_ADD, fdesc->fd, &fdesc->event) != 0)
	    return (-1);
	}
    }
  return (0);
}

static int		_handle_accept(t_anio *server)
{
  int			client_fd;
  struct sockaddr_in	client_sin;
  socklen_t		client_addrlen;

  client_addrlen = sizeof(struct sockaddr_in);
  client_fd = accept(server->fdesc.fd, (struct sockaddr *)&client_sin, &client_addrlen);
  if (client_fd == -1)
    {
      print_err(errno);
      close(server->fdesc.fd);
      return (-1);
   }
  if (libanio_add_client(server, client_fd) != 0)
    {
      close(client_fd);
      return (-1);
    }
  server->fptr_on_accept(server, client_fd);
  return (0);
}

int			libanio_handle_event(t_anio *server, struct epoll_event *job)
{
  if (job->data.fd == server->fdesc.fd)
    {
      if ((job->events & EPOLLERR)
	  || (job->events & EPOLLHUP))
	{
	  DEBUG(GREEN, "TODO: the server seems dead, handle it.\n");
	  /* todo: notify the monitor that it has to stop, closing all clients connexions and destroying all workers */
	  return (-1);
	}
      else if (job->events & EPOLLIN)
	return (_handle_accept(server));
    }
  else
    {
      /* printf("=> (in %lu), ERR:%d HUP:%d IN:%d OUT:%d\n", */
      /*	     pthread_self(), */
      /*	     (job->events & EPOLLERR), */
      /*	     (job->events & EPOLLHUP), */
      /*	     (job->events & EPOLLIN), */
      /*	     (job->events & EPOLLOUT)); */
      if (job->events & EPOLLERR)
	return (_handle_error(server, job->data.fd, 0));
      else if (job->events & EPOLLHUP)
	return (_handle_eof(server, job->data.fd));
      else if (job->events & EPOLLIN)
	return (_handle_read(server, job->data.fd));
      else if (job->events & EPOLLOUT)
	return (_handle_write(server, job->data.fd));
    }
  return (-1);
}
