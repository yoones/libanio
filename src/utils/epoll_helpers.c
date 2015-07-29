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

#include <sys/epoll.h>
#include <stdio.h>
#include "libanio.h"

int		x_epoll_create1(int flags)
{
  int		ret;

  ret = epoll_create1(flags);
  if (ret != 0)
    print_err(errno);
  return (ret);
}

int		x_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
  int		ret;

  ret = epoll_ctl(epfd, op, fd, event);
  if (ret != 0)
    print_err(errno);
  return (ret);
}

int		x_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
  int		ret;
  int		errno_backup;

  do
    {
      ret = epoll_wait(epfd, events, maxevents, timeout);
      errno_backup = errno;
      if (ret < 0)
	print_err(errno);
    } while (ret < 0 && errno_backup == EINTR);
  if (ret < 0)
    print_err(errno_backup);
  return (ret);
}
