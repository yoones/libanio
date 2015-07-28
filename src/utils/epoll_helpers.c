#include <sys/epoll.h>
#include <stdio.h>
#include "libanio.h"

int		x_epoll_create1(int flags)
{
  int		ret;

  DEBUG_IN();
  ret = epoll_create1(flags);
  if (ret != 0)
    print_err(errno);
  DEBUG_OUT();
  return (ret);
}

int		x_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
  int		ret;

  DEBUG_IN();
  ret = epoll_ctl(epfd, op, fd, event);
  if (ret != 0)
    print_err(errno);
  DEBUG_OUT();
  return (ret);
}

int		x_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
  int		ret;
  int		errno_backup;

  DEBUG_IN();
  do
    {
      ret = epoll_wait(epfd, events, maxevents, timeout);
      errno_backup = errno;
      if (ret < 0)
	print_err(errno);
    } while (ret < 0 && errno_backup == EINTR);
  if (ret < 0)
    print_err(errno_backup);
  DEBUG_OUT();
  return (ret);
}
