/*
**
**
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
#include "libanio.h"

void		on_accept(t_anio *server, t_anio_fd *fd)
{
  printf("Server (fd:%d), I got a new client (fd:%d)!\n", server->fd, fd->fd);
}

void		on_read(t_anio *server, t_anio_fd *fd, char *buf, size_t size)
{
  (void)buf;
  printf("Server (fd:%d), I received %d byte(s) from client (fd:%d)\n", server->fd, (int)size, fd->fd);
}

int			start_server(int port)
{
  int			fd;
  struct sockaddr_in	sin;

  /* create server socket */
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1)
    {
      perror("");
      return (-1);
    }
  /* bind */
  memset(&sin, 0, sizeof(struct sockaddr_in));
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  if (bind(fd, (struct sockaddr *)&sin, sizeof(struct sockaddr)) == -1)
    {
      perror("");
      close(fd);
      return (-1);
    }
  /* go in listen mode */
  if (listen(fd, 0) == -1)
    {
      perror("");
      close(fd);
      return (-1);
    }
  return (fd);
}

int		main()
{
  int		fd;
  t_anio	server;

  fd = start_server(5555);
  if (fd == -1)
    return (EXIT_FAILURE);
  assert(libanio_init(&server, fd, 1000, 21, &on_accept, NULL, NULL, NULL, ANIO_MODE_STREAM) == -1);
  assert(libanio_init(&server, fd, 1000, 21, &on_accept, &on_read, NULL, NULL, ANIO_MODE_STREAM) == 0);
  assert(libanio_start_monitor(&server) == 0);
  assert(libanio_start_monitor(&server) == -1);

  /* start libanio loop here */
  pause();

  assert(libanio_stop_monitor(&server) == 0);
  assert(libanio_stop_monitor(&server) == -1);
  close(fd);
  return (EXIT_SUCCESS);
}