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

/* static char		*_extract_bytes(t_fdesc *fdesc, size_t size) */
/* { */
/*   int			ret; */
/*   int			i; */

/* } */

static int		_handle_error(t_anio *server, int fd, int errnumber)
{
  DEBUG_IN();
  print_err(errnumber);
  server->fptr_on_error(server, fd, errnumber);
  libanio_remove_client(server, fd);
  return (0);
}

static int		_handle_eof(t_anio *server, int fd)
{
  DEBUG_IN();
  /* todo: extract what's left in client's readbuf */
  server->fptr_on_eof(server, fd, NULL, 0);
  libanio_remove_client(server, fd);
  return (0);
}

static int		_handle_read(t_anio *server, int fd)
{
  char			buff[4096];
  int			ret;
  char			*extract;
  t_fdesc		*fdesc;
  t_anio_buf		*aniobuf;

  DEBUG_IN();
  ret = read(fd, buff, 4096);
  DEBUG(RED, "DEBUG: read() returned %d", ret);
  switch (ret)
    {
    case (-1):
      return (_handle_error(server, fd, errno));
    case (0):
      /* todo: extract unread data, send it to on_eof callback */
      server->fptr_on_eof(server, fd, NULL, 0); /* buf NULL size 0 for debug */
      libanio_remove_client(server, fd);
      break ;
    default:
      return (0);
      /* stack data or call callback depending on the read mode (stream, block or line) */
      switch (server->mode)
	{
	case (ANIO_MODE_STREAM):
	  printf("THAT'S A GOOD START\n");
	  if (!(extract = malloc(sizeof(char) * ret)))
	    return (_handle_error(server, fd, errno));
	  memcpy(extract, buff, ret);
	  server->fptr_on_read(server, fd, extract, ret);
	  break;
	/* case (ANIO_MODE_BLOCK): */
	/*   if (!(aniobuf = malloc(sizeof(t_anio_buf))) */
	/*       || !(aniobuf->data = malloc(sizeof(char) * ret))) */
	/*     { */
	/*       free(aniobuf); */
	/*       return (_handle_error(server, fd, errno)); */
	/*     } */
	/*   aniobuf->memsize = ret; */
	/*   memcpy(aniobuf->data, buff, ret); */
	/*   if (libanio_get_client(server, fd, &fdesc) != 0) */
	/*     { /\* TODO : handle error here *\/ } */
	/*   if (list_push_back(&fdesc->readbuf, aniobuf)) */
	/*     { */
	/*       print_err(errno); */
	/*       free(aniobuf->data); */
	/*       free(aniobuf); */
	/*       return (-1);	/\* -1 or 0 ??? *\/ */
	/*     } */
	/*   /\* TODO : t_anio_buf = extract_block(fdesc, size); *\/ */
	/*   server->fptr_on_read(server, fd, aniobuf->data, aniobuf->memsize); */
	/*   /\* data is to be freed by the read handler *\/ */
	/*   free(aniobuf); */
	/*   break; */
	/* case (ANIO_MODE_LINE): */
	/*   /\* TODO *\/ */
	/*   printf("TODO mode ANIO_MODE_LINE\n"); */
	/*   abort(); */
	/*   break; */
	/* default: */
	/*   DEBUG(RED, "Error: unknown reading mode (%d), abort!!", server->mode); */
	/*   abort(); */
	/*   return (-1); */
	}
    }
  return (0);
}

static int		_handle_write(t_anio *server, int fd)
{
  DEBUG_IN();
  DEBUG(GREEN, "DEBUG: worker -> _handle_write");
  (void)server;
  DEBUG(GREEN, "TODO: handle write on fd %d\n", fd);
  return (-1);
}

static int		_handle_accept(t_anio *server)
{
  int			client_fd;
  struct sockaddr_in	client_sin;
  socklen_t		client_addrlen;

  DEBUG_IN();
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
  DEBUG_IN();
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
      printf("=> (in %lu), ERR:%d HUP:%d IN:%d OUT:%d\n",
	     pthread_self(),
	     (job->events & EPOLLERR),
	     (job->events & EPOLLHUP),
	     (job->events & EPOLLIN),
	     (job->events & EPOLLOUT));
      if (job->events & EPOLLERR)
	return (_handle_error(server, job->data.fd, 0));
      else if (job->events & EPOLLHUP)
	return (_handle_eof(server, job->data.fd));
      else if (job->events & EPOLLIN)
	return (_handle_read(server, job->data.fd));
      else if (job->events & EPOLLOUT)
	return (_handle_write(server, job->data.fd));
    }
  DEBUG(GREEN, "DEBUG: worker -> no handler set for this case");
  return (-1);
}
