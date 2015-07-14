#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include "list.h"
#include "fdesc.h"

/*
  Called when select() says the fd is available for reading.
  It reads at most (FDESC_READ_BUF_SIZE - 1) bytes,
  stores them into the fdesc's read buffer,
  then calls _fdesc_read_extract_lines() to extract
  as many lines as possible from it.
 */
static int	_fdesc_select_operate_read(t_fdesc *fdesc)
{
  int		ret;
  t_fdesc_buf	*fdesc_buf;
  char		buf[FDESC_READ_BUF_SIZE];

  ret = read(fdesc->fd, buf, FDESC_READ_BUF_SIZE);
  if (ret == -1)
    {
      fdesc->errno_cache = errno;
      return (-1);
    }
  else if (ret == 0)
    {
      fdesc->errno_cache = 0;
      fdesc->eof = 1;
      return (0);
    }
  if (!(fdesc_buf = malloc(sizeof(t_fdesc_buf))))
    {
      fdesc->errno_cache = errno;
      return (-1);
    }
  if (!(fdesc_buf->data = strdup(buf)))
    {
      fdesc->errno_cache = errno;
      free(fdesc_buf);
      return (-1);
    }
  fdesc_buf->size = strlen(buf);
  if (list_push_back(&fdesc->readbuf, fdesc_buf))
    {
      fdesc->errno_cache = errno;
      free(fdesc_buf->data);
      free(fdesc_buf);
      return (-1);
    }
  return (_fdesc_read_extract_lines(fdesc));
}

static int	_fdesc_select_operate_write(t_fdesc *fdesc)
{
  int		ret;
  t_fdesc_buf	*fdesc_buf;

  fdesc_buf = fdesc->writebuf.head->data;
  ret = write(fdesc->fd, fdesc_buf->data, fdesc_buf->size);
  if (ret == -1)
    {
      fdesc->errno_cache = errno;
      return (-1);
    }
  else if (ret == (int)fdesc_buf->size)
    {
      list_pop_front(&fdesc->writebuf);
      fdesc->errno_cache = 0;
      return (0);
    }
  memmove(fdesc_buf->data, fdesc_buf->data + ret, fdesc_buf->size - ret);
  fdesc_buf->size = fdesc_buf->size - ret;
  fdesc->errno_cache = 0;
  return (0);
}

/*
  Takes a linked list of fdescs, monitors them all in reading,
  and in writing for those whose writebuffer contains data.
 */
static int	_fdesc_select_monitor_fds(t_list *fdescs, struct timeval *tv,
					  fd_set *rfds, fd_set *wfds)
{
  int		fdmax = -1;
  t_lnode	*w;
  t_fdesc	*fdesc;

  FD_ZERO(rfds);
  FD_ZERO(wfds);
  for (w = fdescs->head; w != NULL; w = w->next)
    {
      fdesc = w->data;
      FD_SET(fdesc->fd, rfds);
      if (fdesc->fd > fdmax)
	fdmax = fdesc->fd;
      if (fdesc->writebuf.size > 0)
	FD_SET(fdesc->fd, wfds);
    }
  if (fdmax == -1)
    return (0);
  return (select(fdmax + 1, rfds, wfds, NULL, tv));
}

/*
  Takes a linked list of fdescs, monitors them, then operates
  io actions on the file descriptiors that are ready for it.
  Returns (0) on success
  Returns a va lue different from 0 if, during the process, at least one syscall failed
 */
int		fdesc_select(t_list *fdescs, struct timeval *tv)
{
  t_lnode	*w;
  t_fdesc	*fdesc;
  fd_set	rfds;
  fd_set	wfds;
  int		ret;

  if ((ret =_fdesc_select_monitor_fds(fdescs, tv, &rfds, &wfds)) < 1)
    return (ret);
  ret = 0;
  for (w = fdescs->head; w != NULL; w = w->next)
    {
      fdesc = w->data;
      /* TODO ? : if (fdesc->eof == 0) */
      if (FD_ISSET(fdesc->fd, &rfds))
	ret |= _fdesc_select_operate_read(fdesc);
      if (FD_ISSET(fdesc->fd, &wfds))
	ret |= _fdesc_select_operate_write(fdesc);
    }
  return (ret);
}
