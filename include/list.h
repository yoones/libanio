/*
**
**
*/

#ifndef LIST_H_
# define LIST_H_

/* typedef enum		e_ldatatype */
/*   { */
/*     LDT_INT, */
/*     LDT_STRING, */
/*     LDT_ */
/*   }			t_ldatatype; */

typedef struct		s_lnode
{
  void			*data;
  struct s_lnode	*prev;
  struct s_lnode	*next;
}			t_lnode;

typedef int (t_list_data_cmp)(void *, void *);
typedef void (t_list_data_free)(void *);

typedef struct		s_list
{
  t_lnode		*head;
  t_lnode		*tail;
  int			size;
  t_list_data_free	*f_free;
  t_list_data_cmp	*f_cmp;
}			t_list;

void		list_init(t_list *list,
			  t_list_data_free *f_free,
			  t_list_data_cmp *f_cmp);

t_list		*list_create(t_list_data_free *f_free,
			     t_list_data_cmp *f_cmp);

void		list_clear(t_list *list);

inline void	lnode_init(t_lnode *node,
			   void *data, t_lnode *prev, t_lnode *next);

t_lnode		*lnode_create(void *data, t_lnode *prev, t_lnode *next);

int		list_push_front(t_list *list, void *data);

int		list_push_back(t_list *list, void *data);

void		list_pop_front(t_list *list);

void		list_pop_back(t_list *list);

void		list_pop_node(t_list *list, t_lnode *node);

void		list_pop_data(t_list *list, void *data);

int		list_contains(t_list *list, void *data);

#endif /* LIST_H_ */
