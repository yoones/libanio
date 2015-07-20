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
#include <signal.h>
#include "libanio.h"

void		on_accept(t_anio *server, int fd)
{
  printf("Server (fd:%d), I got a new client (fd:%d)!\n", server->fdesc.fd, fd);
}

void		on_read(t_anio *server, int fd, char *buf, size_t size)
{
  (void)buf;
  printf("Server (fd:%d), I received %d byte(s) from client (fd:%d)\n", server->fdesc.fd, (int)size, fd);
}

void		on_eof(t_anio *server, int fd, char *buf, size_t size)
{
  (void)buf;
  (void)size;
  printf("Server (fd:%d), client (fd:%d) is gone!\n", server->fdesc.fd, fd);
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

void		_handle_sigint(int sig)
{
  (void)sig;
  fsync(1);
  fsync(2);
  write(1, RESET, strlen(RESET));
  exit(0);
}

int		main(int argc, char **argv)
{
  int		fd;
  t_anio	server;
  int		port;

  signal(SIGINT, &_handle_sigint);
  port = 5555;
  if (argc == 2)
    port = atoi(argv[1]);
  printf("Server listening on port %d\nPress [ENTER] to continue...\n", port);
  getchar();
  fd = start_server(port);
  if (fd == -1)
    return (EXIT_FAILURE);
  /* assert(libanio_init(&server, fd, 1000, 21, &on_accept, NULL, NULL, NULL, ANIO_MODE_STREAM) == -1); */
  assert(libanio_init(&server, fd, 1000, 1, &on_accept, &on_read, &on_eof, NULL, ANIO_MODE_STREAM) == 0);
  assert(libanio_start_monitor(&server) == 0);
  /* assert(libanio_start_monitor(&server) == -1); */

  /* start libanio loop here */
  pause();

  assert(libanio_stop_monitor(&server) == 0);
  /* assert(libanio_stop_monitor(&server) == -1); */
  close(fd);
  return (EXIT_SUCCESS);
}
