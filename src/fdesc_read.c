#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include "list.h"
#include "fdesc.h"

static int	_find_eol(t_fdesc_buf *fdesc_buf, char *eol)
{
  int		i;
  int		eol_len;

  eol_len = strlen(eol);
  for (i = 0; i < fdesc_buf->size; i++)
    {
      if (!strncmp(fdesc_buf->data + i, eol, eol_len))
	return (i);
    }
  return (-1);
}

size_t		fdesc_bytes_available(t_fdesc *fdesc)
{
  size_t	i;
  t_lnode	*w;
  t_fdesc_buf	fdesc_buf;

  i = 0;
  for (w = fdesc->readbuf.head; w; w = w->next)
    {
      fdesc_buf = w->data;
      i += fdesc_buf->size;
    }
  return (i);
}

int		fdesc_is_block_available(t_fdesc *fdesc, size_t size)
{
  size_t	i;
  t_lnode	*w;
  t_fdesc_buf	fdesc_buf;

  i = 0;
  for (w = fdesc->readbuf.head; w; w = w->next)
    {
      fdesc_buf = w->data;
      i += fdesc_buf->size;
      if (i >= size)
	return (1);
    }
  return (0);
}

int		fdesc_is_line_available(t_fdesc *fdesc)
{
  t_lnode	*w;
  t_fdesc_buf	fdesc_buf;

  for (w = fdesc->readbuf.head; w; w = w->next)
    {
      fdesc_buf = w->data;
      if (_find_eol(fdesc_buf, fdesc->eol) != -1)
	return (1);
    }
  return (0);
}

/*
  Expects fdesc_is_block_available() to be called beforehand (and that it said yes)
 */
int		fdesc_read_block(t_fdesc *fdesc, char **buf, size_t size)
{
  size_t	i;
  t_fdesc_buf	fdesc_buf;
  char		*tmp;

  i = fdesc_bytes_available(fdesc);
  if (i == 0)
    {
      fdesc->errno_cache = EAGAIN;
      return (0);
    }
  if (size > i)
    size = i;
  else
    i = size;
  if (!(tmp = malloc(sizeof(char) * (*size))))
    return (-1);
  *buf = tmp;
  while (i > 0 && fdesc->readbuf.head)
    {
      fdesc_buf = fdesc->readbuf.head->data;
      if (i >= fdesc_buf->size)
	{
	  memcpy(tmp, fdesc_buf->data, fdesc_buf->size);
	  tmp += fdesc_buf->size;
	  i -= fdesc_buf->size;
	  /* todo: free fdesc_buf content and fdesc_buf */
	  list_pop_first(&fdesc->readbuf);
	}
      else
	{
	  memcpy(tmp, fdesc_buf->data, i);
	  fdesc_buf->size -= i;
	  memmove(fdesc_buf->data, fdesc_buf->data + i, fdesc_buf->size);
	  i = 0;
	}
    }
  return (size);
}

/*
  Expects fdesc_is_line_available() to be called beforehand (and that it said yes)
 */
int		fdesc_read_line(t_fdesc *fdesc, char **buf, size_t *size);
{
  t_lnode	*w;
  t_fdesc_buf	fdesc_buf;
  int		nbr_blocks = 0;
  int		remaining = 0;

  *size = 0;
  for (w = fdesc->readbuf.head; w; w = w->next)
    {
      fdesc_buf = w->data;
      if ((remaining = _find_eol(fdesc_buf->data, fdesc_buf->eol) == -1))
	{
	  (*size)++;
	  nbr_blocks++;
	}
      else
	{
	  (*size) += remaining + strlen(fdesc->eol);
	  break ;
	}
    }
  return (fdesc_read_block(fdesc, buf, *size))
}












/* ######### OLD ########### */


/* /\* */
/*   Returns a line if one is available. */
/*   fdesc_is_line_available() should always be called before fdesc_readline(). */
/*  *\/ */
/* int		fdesc_readline(t_fdesc *fdesc, char **buf) */
/* { */
/*   if (fdesc->lines_available.size == 0) */
/*     { */
/*       fdesc->errno_cache = EWOULDBLOCK; */
/*       return (-1); */
/*     } */
/*   *buf = fdesc->lines_available.head->data; */
/*   list_pop_front(&fdesc->lines_available); */
/*   fdesc->errno_cache = 0; */
/*   return (0); */
/* } */

/* /\* */
/*   Tells if at least one line is available. */
/*   It also says if a call to fdesc_readline() will succeed. */
/*  *\/ */
/* inline int	fdesc_is_line_available(t_fdesc *fdesc) */
/* { */
/*   return (fdesc->lines_available.size > 0); */
/* } */

/* /\* */
/*   Extracts lines ended by FDESC_LINE_SEP into a separate */
/*   linked list for them to be read using fdesc_readline(). */
/*  *\/ */
/* #define FDESC_LINE_SEP "\n" */
/* static int	_fdesc_read_extract_line(t_fdesc *fdesc) */
/* { */
/*   int		nbr_blocks = 0; */
/*   int		i, j; */
/*   t_lnode	*w; */
/*   t_fdesc_buf	*fdesc_buf; */
/*   t_fdesc_buf	new_buf; */

/*   if (fdesc->readbuf.size == 0) */
/*     return (0); */
/*   new_buf.data = NULL; */
/*   new_buf.size = 0; */
/*   i = -1; */
/*   for (w = fdesc->readbuf.head; w != NULL; w = w->next, nbr_blocks++) */
/*     { */
/*       fdesc_buf = w->data; */
/*       if ((i = _find_char(fdesc_buf->data, FDESC_LINE_SEP)) != -1) */
/*	break ; */
/*       new_buf.size += fdesc_buf->size; */
/*     } */
/*   if (i == -1) */
/*     return (0); */
/*   new_buf.size += i; */
/*   if (!(new_buf.data = malloc(sizeof(char) * (new_buf.size + 1)))) */
/*     { */
/*       fdesc->errno_cache = errno; */
/*       return (-1); */
/*     } */
/*   /\* extraction starts here *\/ */
/*   /\* 1. copy blocks *\/ */
/*   j = 0; */
/*   while (nbr_blocks > 0) */
/*     { */
/*       fdesc_buf = fdesc->readbuf.head->data; */
/*       strcpy(new_buf.data + j, fdesc_buf->data); */
/*       j += fdesc_buf->size; */
/*       free(fdesc_buf); */
/*       list_pop_front(&fdesc->readbuf); */
/*       nbr_blocks--; */
/*     } */
/*   /\* 2. copy remaining (until separator) *\/ */
/*   fdesc_buf = fdesc->readbuf.head->data; */
/*   strncpy(new_buf.data + j, fdesc_buf->data, i); */
/*   new_buf.data[j + i] = '\0'; */
/*   /\* 3. remove extracted content from buffer *\/ */
/*   if (fdesc_buf->data[i + strlen(FDESC_LINE_SEP)] != '\0') */
/*     { */
/*       fdesc_buf->size = strlen(fdesc_buf->data + i + strlen(FDESC_LINE_SEP)); */
/*       memmove(fdesc_buf->data, fdesc_buf->data + i + strlen(FDESC_LINE_SEP), fdesc_buf->size); */
/*     } */
/*   else */
/*     { */
/*       fdesc_buf = fdesc->readbuf.head->data; */
/*       free(fdesc_buf->data); */
/*       free(fdesc_buf); */
/*       list_pop_front(&fdesc->readbuf); */
/*     } */
/*   /\* 4. put extracted line into lines_available *\/ */
/*   if (list_push_back(&fdesc->lines_available, new_buf.data)) */
/*     { */
/*       fdesc->errno_cache = errno; */
/*       free(new_buf.data); */
/*       return (-1); */
/*     } */
/*   fdesc->errno_cache = 0; */
/*   return (0); */
/* } */

/* /\* */
/*   Loop calling _fdesc_read_extract_line() as many times as needed. */
/*  *\/ */
/* static int	_fdesc_read_extract_lines(t_fdesc *fdesc) */
/* { */
/*   int		lines; */
/*   int		ret; */

/*   lines = fdesc->lines_available.size; */
/*   while ((ret = _fdesc_read_extract_line(fdesc)) == 0) */
/*     { */
/*       if (lines == fdesc->lines_available.size) */
/*	break ; */
/*       lines = fdesc->lines_available.size; */
/*     } */
/*   return (ret); */
/* } */
