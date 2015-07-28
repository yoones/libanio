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

#ifndef LIST_H_
# define LIST_H_

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
