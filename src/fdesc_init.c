#include "libanio.h"

int		libanio_fdesc_init(t_fdesc *fdesc, int fd)
{
  if (fd == -1)
    close(fd);
  fdesc->fd = fd;
  fdesc->closed = 0;
  list_init(&fdesc->readbuf, NULL, NULL);
  list_init(&fdesc->writebuf, NULL, NULL);
  return (0);
}

int		libanio_fdesc_close(t_fdesc *fdesc)
{
  if (fdesc->fd != -1)
    close(fdesc->fd);
  fdesc->fd = -1;
  fdesc->closed = 1;
  list_clear(&fdesc->readbuf);
  list_clear(&fdesc->writebuf);
}

t_fdesc		*libanio_get_fdesc(t_anio *server, int fd)
{
  t_lnode	*w;
  t_fdesc	*fdesc;

  for (w = server->clients.head; w != NULL; w = w->next)
    {
      fdesc = w->data;
      if (fdesc->fd == fd)
	return (fdesc);
    }
  return (NULL);
}

int		libanio_has_client(t_anio *server, int fd)
{
  return (libanio_get_fdesc(server, fd) == NULL ? 0 : 1);
}

int		libanio_add_client(t_anio *server, int fd)
{
  t_fdesc	*fdesc;

  if (libanio_has_client(server, fd) == 1)
    return (-1);
  if (!(fdesc = malloc(sizeof(t_fdesc))))
    {
      perror(NULL);
      return (-1);
    }
  if (libanio_fdesc_init(fdesc, fd) == -1)
    return (-1);
  return (0);
}

int		libanio_remove_client(t_anio *server, int fd)
{
  t_lnode	*w, prev_w;
  t_fdesc	*fdesc;

  if (server->clients.size == 0)
    return (-1);
  fdesc = server->clients.head->data;
  if (fdesc->fd == fd)
    {
      list_pop_front(&server->clients);
      return (0);
    }
  fdesc = server->clients.tail->data;
  if (fdesc->fd == fd)
    {
      list_pop_back(&server->clients);
      return (0);
    }
  if (server->clients.size > 1)
  for (w = server->clients.head; w != NULL; w = w->next)
    {
      fdesc = w->next->data;
      if (fdesc->fd == fd)
	{
	  /* remove here */
	  return (0);
	}
    }
  return (-1);
}

int		libanio_close_client(t_anio *server, int fd)
{

}
