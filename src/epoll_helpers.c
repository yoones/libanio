#include <sys/epoll.h>
#include <stdio.h>

int		x_epoll_create1(int flags)
{
  int		ret;

  ret = epoll_create1(flags);
  if (ret != 0)
    perror(NULL);
  return (ret);
}

int		x_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
  int		ret;

  ret = epoll_ctl(epfd, op, fd, event);
  if (ret != 0)
    perror(NULL);
  return (ret);
}
