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

/**
 * @file
 */

/** @struct s_lnode
 *  @brief This structure holds a double linked node
 *  @var s_lnode::data
 *  Member 'data' contains the data stored in this node
 *  @var s_lnode::prev
 *  Member 'prev' contains the address of the previous node (or NULL if there isn't any)
 *  @var s_lnode::next
 *  Member 'next' contains the address of the next node (or NULL if there isn't any)
 */
typedef struct		s_lnode
{
  void			*data;
  struct s_lnode	*prev;
  struct s_lnode	*next;
}			t_lnode;

typedef int (t_list_data_cmp)(void *, void *);
typedef void (t_list_data_free)(void *);

/** @struct s_list
 *  @brief This structure holds a node if the double linked list
 *  @var s_list::head
 *  Member 'head' contains the address of the first node (of NULL if there isn't any)
 *  @var s_list::tail
 *  Member 'tail' contains the address of the last node (of NULL if there isn't any)
 *  @var s_list::size
 *  Member 'size' contains the number of nodes stored in the linked list
 *  @var s_list::f_free
 *  Member 'f_free' contains the address of a function of call for freeing 'data' when cleaning the list (ex: free)
 *  @var s_list::f_cmp
 *  Member 'f_cmp' contains the address of a function of call for comparing two nodes' 'data' (ex: strcmp)
 */
typedef struct		s_list
{
  t_lnode		*head;
  t_lnode		*tail;
  int			size;
  t_list_data_free	*f_free;
  t_list_data_cmp	*f_cmp;
}			t_list;

/**
 * @brief Initializes a t_list structure.
 *
 * Exemples:
 *
 * list_init(&list, (t_list_data_free *)&free, (t_list_data_cmp *)&strcmp);
 *
 * @param list Pointer to the t_list structure to initialize.
 * @param f_free Pointer to a function that is able to free 'data' and its contect if any.
 * @param f_cmp Pointer to a function that is able to compare two nodes' 'data'.
 */
void		list_init(t_list *list,
			  t_list_data_free *f_free,
			  t_list_data_cmp *f_cmp);

/**
 * @brief Allocates and initializes a t_list structure.
 *
 * Exemples:
 *
 * list = list_create((t_list_data_free *)&free, (t_list_data_cmp *)&strcmp);
 *
 * @param f_free Pointer to a function that is able to free 'data' and its contect if any.
 * @param f_cmp Pointer to a function that is able to compare two nodes' 'data'.
 *
 * @return The address of the list on success
 * @return (NULL) on error
 */
t_list		*list_create(t_list_data_free *f_free,
			     t_list_data_cmp *f_cmp);

/**
 * @brief Pops all nodes in the list. If f_free is set, it is called on each node's 'data' before freeing the node.
 *
 * Exemples:
 *
 * list_clear(&list);
 *
 * @param list Pointer to the t_list structure to initialize.
 */
void		list_clear(t_list *list);

/**
 * @brief Initializes a t_lnode structure.
 *
 * Exemples:
 *
 * lnode_init(&node, "random data", NULL, NULL);
 *
 * @param node Pointer to the t_lnode structure to initialize.
 * @param data Data to store in the node.
 * @param prev Pointer to the previous node.
 * @param next Pointer to the next node.
 */
void		lnode_init(t_lnode *node,
			   void *data, t_lnode *prev, t_lnode *next);

/**
 * @brief Allocates and initializes a t_lnode structure.
 *
 * Exemples:
 *
 * node = lnode_create("random data", NULL, NULL);
 *
 * @param data Data to store in the node.
 * @param prev Pointer to the previous node.
 * @param next Pointer to the next node.
 *
 * @return The address of the node on success
 * @return (NULL) on error
 */
t_lnode		*lnode_create(void *data, t_lnode *prev, t_lnode *next);

/**
 * @brief Pushes a data at the beginning of the list.
 *
 * Exemples:
 *
 * list_push_front(&list, "random data");
 *
 * @param list List in which to store the data.
 * @param data Data to store.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		list_push_front(t_list *list, void *data);

/**
 * @brief Pushes a data at the end of the list.
 *
 * Exemples:
 *
 * list_push_back(&list, "random data");
 *
 * @param list List in which to store the data.
 * @param data Data to store.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		list_push_back(t_list *list, void *data);

/**
 * @brief Removes the first node of the list.
 * Warning: f_free is NOT called on node->data
 *
 * Exemples:
 *
 * list_pop_front(&list);
 *
 * @param list List from which to pop the node.
 */
void		list_pop_front(t_list *list);

/**
 * @brief Removes the last node of the list.
 * Warning: f_free is NOT called on node->data
 *
 * Exemples:
 *
 * list_pop_bask(&list);
 *
 * @param list List from which to pop the node.
 */
void		list_pop_back(t_list *list);

/**
 * @brief Removes the node from the list if the node is found in it.
 * Warning: f_free is NOT called on node->data
 *
 * Exemples:
 *
 * list_pop_node(&list, &node);
 *
 * @param list List from which to pop the node.
 */
void		list_pop_node(t_list *list, t_lnode *node);

/**
 * @brief Removes the first node that holds data from the list if this data is found in it.
 * Warning: f_free is NOT called on node->data
 *
 * Exemples:
 *
 * list_pop_data(&list, &data);
 *
 * @param list List from which to pop the node.
 */
void		list_pop_data(t_list *list, void *data);

/**
 * @brief Checks if he list contains a node that contains the given data
 *
 * Exemples:
 *
 * if (list_contains(&list, &data) == 0)
 *
 * @param list List from which to pop the node.
 */
int		list_contains(t_list *list, void *data);

#endif /* LIST_H_ */
