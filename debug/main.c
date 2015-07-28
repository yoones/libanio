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
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include "libanio.h"

#define NB_WORKERS 1

t_anio	server;

void		on_accept(t_anio *server, int fd)
{
  dprintf(2, "Server (fd:%d), I got a new client (fd:%d)!\n", server->fdesc.fd, fd);
}

void		on_read(t_anio *server, int fd, char *buf, size_t size)
{
  dprintf(2, "Server (fd:%d), I received %d byte(s) from client (fd:%d)\n", server->fdesc.fd, (int)size, fd);
  dprintf(2, "===\n%s\n===\n", buf);
}

void		on_eof(t_anio *server, int fd, char *buf, size_t size)
{
  (void)buf;
  (void)size;
  dprintf(2, "Server (fd:%d), client (fd:%d) is gone!\n", server->fdesc.fd, fd);
}

int			start_server(int port)
{
  int			fd;
  struct sockaddr_in	sin;

  /* create server socket */
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1)
    {
      perror(NULL);
      return (-1);
    }
  /* bind */
  memset(&sin, 0, sizeof(struct sockaddr_in));
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  if (bind(fd, (struct sockaddr *)&sin, sizeof(struct sockaddr)) == -1)
    {
      perror(NULL);
      close(fd);
      return (-1);
    }
  /* go in listen mode */
  if (listen(fd, 0) == -1)
    {
      perror(NULL);
      close(fd);
      return (-1);
    }
  return (fd);
}

/* void		_handle_sigint(int sig) */
/* { */
/*   (void)sig; */
/*   fsync(1); */
/*   fsync(2); */
/*   write(1, RESET, strlen(RESET)); */
/*   libanio_stop_monitor(&server); */
/*   libanio_free(&server); */
/*   exit(0); */
/* } */

int		main(int argc, char **argv)
{
  int		fd;
  int		port;

  /* signal(SIGINT, &_handle_sigint); */
  port = 5555;
  if (argc == 2)
    port = atoi(argv[1]);
  printf("Server listening on port %d\nPress [ENTER] to continue...\n", port);
  getchar();
  fd = start_server(port);
  if (fd == -1)
    return (EXIT_FAILURE);
  /* assert(libanio_init(&server, fd, 1000, NB_WORKERS, &on_accept, &on_read, &on_eof, NULL, ANIO_MODE_STREAM) == 0); */
  assert(libanio_init(&server, fd, 1000, NB_WORKERS, &on_accept, &on_read, &on_eof, NULL, ANIO_MODE_LINE, "\n") == 0);
  assert(libanio_start_monitor(&server) == 0);

  /* start libanio loop here */
  sleep(500);

  libanio_stop_monitor(&server);
  libanio_free(&server);
  close(fd);
  return (EXIT_SUCCESS);
}
