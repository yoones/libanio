/*
**
**
*/

#include <string.h>
#include <stdlib.h>
#include "list.h"

/*
  Initializes a list
 */
void		list_init(t_list *list,
			  t_list_data_free *f_free,
			  t_list_data_cmp *f_cmp)
{
  memset(list, '\0', sizeof(t_list));
  list->f_free = f_free;
  list->f_cmp = f_cmp;
}

/*
  Allocates and initializes a new list
 */
t_list		*list_create(t_list_data_free *f_free,
			     t_list_data_cmp *f_cmp)
{
  t_list	*list;

  list = malloc(sizeof(t_list));
  if (!list)
    return (NULL);
  list_init(list, f_free, f_cmp);
  return (list);
}

/*
  Pops all nodes in (list)
  If (fptr) is set, it is called on every node's data to free it
 */
void		list_clear(t_list *list)
{
  if (!list->f_free)
    while (list->size > 0)
      list_pop_front(list);
  else
    while (list->size > 0)
      {
	list->f_free(list->head->data);
	list_pop_front(list);
      }
}

/*
  Initializes a node
 */
inline void	lnode_init(t_lnode *node,
			   void *data, t_lnode *prev, t_lnode *next)
{
  node->data = data;
  node->prev = prev;
  node->next = next;
}

/*
  Allocates and initializes a new node
 */
t_lnode		*lnode_create(void *data, t_lnode *prev, t_lnode *next)
{
  t_lnode	*node;

  node = malloc(sizeof(t_lnode));
  if (!node)
    return (NULL);
  lnode_init(node, data, prev, next);
  return (node);
}

/*
  Adds node with (data) at the beginning of (list)
  Returns 0 on success
  Returns -1 on failure
*/
int		list_push_front(t_list *list, void *data)
{
  t_lnode	*node;

  if (!(node = malloc(sizeof(t_lnode))))
    return (-1);
  node->data = data;
  node->prev = NULL;
  node->next = list->head;
  if (list->head)
    list->head->prev = node;
  list->head = node;
  if (!list->tail)
    list->tail = node;
  list->size++;
  return (0);
}

/*
  If (list) is empty, calls list_push().
  Otherwise, adds node with (data) at the end of (list)
  Returns 0 on success
  Returns -1 on failure
*/
int		list_push_back(t_list *list, void *data)
{
  t_lnode	*node;

  if (list->size == 0)
    return (list_push_front(list, data));
  if (!(node = malloc(sizeof(t_lnode))))
    return (-1);
  node->data = data;
  node->prev = list->tail;
  node->next = NULL;
  list->tail->next = node;
  list->tail = node;
  list->size++;
  return (0);
}

/*
  If (list) is not empty,
  Frees first node of (list)
  Expects data to be already freed
 */
void		list_pop_front(t_list *list)
{
  t_lnode	*node;

  if (list->size > 0)
    {
      node = list->head;
      list->head = node->next;
      if (list->head)
	list->head->prev = NULL;
      free(node);
      list->size--;
    }
  if (!list->size)
    {
      list->head = NULL;
      list->tail = NULL;
    }
}

/*
  If (list) is not empty,
  Frees last node of (list)
  Expects data to be already freed
 */
void		list_pop_back(t_list *list)
{
  t_lnode	*node;

  if (list->size < 2)
    {
      list_pop_front(list);
      return ;
    }
  node = list->tail;
  list->tail = node->prev;
  if (list->tail)
    list->tail->next = NULL;
  free(node);
  list->size--;
}

/*
  Compares data using (list->f_cmp)
  Returns 0 on success
  Returns -1 on failure or if no comparison function is set (list->f_cmp)
 */
int		list_contains(t_list *list, void *data)
{
  t_lnode	*w;

  if (!list->f_cmp)
    return (-1);
  w = list->head;
  while (w && list->f_cmp(w->data, data))
    w = w->next;
  return (w ? 0 : -1);
}
