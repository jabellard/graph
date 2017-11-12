#include <stdlib.h>
#include <stdio.h>
#include "list.h"

list_t * 
list_create(data_dtor_func_t dtor, match_func_t match)
{
	list_t *l = malloc(sizeof(list_t));
	if (!l)
	{
		return NULL;
	} // end if
	
	l->head = NULL;
	l->tail = NULL;
	l->len = 0;
	l->data_dtor = dtor;
	l->match = match;
	return l;
} // end list_create()

int 
list_destroy(list_t *l)
{
	if (!l || !l->data_dtor)
	{
		return -1;
	} // end if
	
	list_node_t *curr = l->head;
	list_node_t *next = NULL;
	
	while ((l->len)--)
	{
		next = curr->next;
		int res = list_node_destroy(curr);
		if (res == -1)
		{
			return -1;
		} // end if
		curr = next;
	} // end while
	
	sfree(l);
	return 0;
} // end list_destroy()

list_node_t *
list_push_back(list_t *l, list_node_t *n)
{
	if (!l || !n)
	{
		return NULL;
	} // end if
	
	if (l->len)
	{
		n->prev = l->tail;
		n->next = NULL;
		l->tail->next = n;
		l->tail = n;
	} // end if
	else
	{
		n->prev = NULL;
		n->next = NULL;
		l->head = n;
		l->tail = n;
	} // end else
			
	n->container = l;
	(l->len)++;
	return n;
} // end list_push_back()

list_node_t *
list_pop_back(list_t *l)
{
	if (!l->len)
	{
		return NULL;
	} // end if
	
	list_node_t *n = l->tail;
	if ((l->len) - 1)
	{ 
		l->tail = n->prev;
		l->tail->next = NULL;
	} // end if
	else
	{
		l->head = NULL;
		l->tail = NULL;
	} // end else
	
	(l->len)--;
	n->prev = NULL;
	n->next = NULL;
	return n;
} // end list_pop_back()

int
list_pop_back_and_destroy(list_t *l)
{
	list_node_t *n = list_pop_back(l);
	if (!n)
	{
		return -1;
	} // end if
	
	return list_node_destroy(n);
} // end list_pop_back_and_destroy()

list_node_t *
list_push_front(list_t *l, list_node_t *n)
{
	if (!l || !n)
	{
		return NULL;
	} // end if
	
	if (l->len)
	{
		n->prev = NULL;
		n->next = l->head;
		l->head->prev = n;
		l->head = n;
	} // end if
	else
	{
		n->prev = NULL;
		n->next = NULL;
		l->head = n;
		l->tail = n;
	} //end else
	
	n->container = l;
	(l->len)++;
	return n;
} // end list_push_front()

list_node_t *
list_pop_front(list_t *l)
{
	if (!l->len)
	{
		return NULL;
	} // end if
	
	list_node_t *n = l->head;
	
	if ((l->len) - 1)
	{
		l->head = n->next;
		l->head->prev = NULL;
	} // end if
	else
	{
		l->head = NULL;
		l->tail = NULL;
	} // end else
	
	(l->len)--;
	n->prev = NULL;
	n->next = NULL;
	return n;
} // end list_pop_front()

int
list_pop_front_and_destroy(list_t *l)
{
	list_node_t *n = list_pop_front(l);
	if (!n)
	{
		return -1;
	} // end if
	
	return list_node_destroy(n);
} // end list_pop_front_and_destroy()

list_node_t *
list_remove_node(list_node_t *n)
{
	if (!n || !n->container)
	{
		return NULL;
	} // end if
	else if (n->container->head == n)
	{
		return list_pop_front(n->container);
	} // else if
	else if (n->container->tail == n)
	{
		return list_pop_back(n->container);
	} // end else if
	else
	{
		list_node_t *temp = n->prev;
		if (n->prev)
		{
			n->prev->next = n->next;
			n->prev = NULL;
		} // end if
		
		if (n->next)
		{
			n->next->prev = temp;
			n->next = NULL;
		} // end if
		
		(n->container->len)--;
		return n;
	} // end else
} // end list_remove_node()

int 
list_remove_and_destroy_node(list_node_t *n)
{
	list_node_t *node = list_remove_node(n);
	if (!node)
	{
		return -1;
	} // end if
	
	return list_node_destroy(node);
} // end list_remove_and_destroy_node()

list_node_t *
list_insert_before(list_node_t *old, list_node_t *new)
{
	if (!old || !old->container || !new)
	{
		return NULL;
	} // end if
	else if (old->container->head == old)
	{
		return list_push_front(old->container, new);
	} // end else if
	else if (old->container->tail == old)
	{
		if (old->container->len == 1)
		{
			return list_push_front(old->container, new);
			
		} // end if
		else
		{
			list_node_t *temp = old->container->tail->prev;
			new->prev = temp;
			new->next = old->container->tail;
			old->container->tail->prev = new;
			temp->next = new;
			
		} // end else
	} // end else if
	else
	{
		list_node_t *temp = old->prev;
		new->prev = temp;
		new->next = old;
		old->prev = new;
		temp->next = new;
		
	} // end else
	
	(old->container->len)++;
	return new;
} // end list_insert_before()

list_node_t *
list_insert_after(list_node_t *old, list_node_t *new)
{
	if (!old || !old->container || !new)
	{
		return NULL;
	} // end if
	else if (old->container->tail == old)
	{
		return list_push_back(old->container, new);
	} // end else if
	else if (old->container->head == old)
	{
		if (old->container->len == 1)
		{
			return list_push_back(old->container, new);
		} // end if
		else
		{
			list_node_t *temp = old->container->head->next;
			new->prev = old->container->head;
			new->next = temp;
			old->container->head->next = new;
			temp->prev = new;
		} // end else
	} // end else if
	else
	{
		list_node_t *temp = old->next;
		new->prev = old;
		new->next = temp;
		old->next = new;
		temp->prev = new;
	} // end else
	
	(old->container->len)++;
	return new;
} // end list_insert_after()

list_iterator_t *
list_iterator_create(list_t *l, list_direction_t d)
{
	if (l->len == 0)
	{
		return NULL;
	} // end if
	
	list_iterator_t *it = malloc(sizeof(list_iterator_t));
	if (!it)
	{
		return NULL;
	} // end if
	
	it->direction = d;
	
	if (d == HEAD)
	{
		it->next = l->head;
	} // end if
	else
	{
		it->next = l->tail;
	} // end else
	
	return it;
} // end list_iterator_create()

list_node_t *
list_iterator_next(list_iterator_t *it)
{
	list_node_t *curr = it->next;
	
	if (curr)
	{
		if (it->direction == HEAD)
		{
			it->next = curr->next;
		} // end if
		else
		{
			it->next = curr->prev;
		} // end else
	} // end if
	
	return curr;
} // end list_iterator_next()

void 
list_iterator_destroy(list_iterator_t *it)
{
	sfree(it);
} // end list_iterator_destroy()

list_node_t *
list_node_create(void *data)
{
	list_node_t *n = malloc(sizeof(list_node_t));
	if (!n)
	{
		return NULL;
	} // end if
	
	n->container = NULL;
	n->prev = NULL;
	n->next = NULL;
	n->data = data;
	return n;
} // end list_node_create()

int 
list_node_destroy(list_node_t *n)
{
	if (!n || !n->container || !n->container->data_dtor)
	{
		return -1;
	} // end if
	
	n->container->data_dtor(n->data);
	
	sfree(n);
	return 0;
} // end list_node_destroy()

list_node_t *
list_find_node(list_t *l, void *v)
{
	if (!l || !l->match)
	{
		return NULL;
	} // end if
	
	list_iterator_t *it = list_iterator_create(l, HEAD);
	if (!it)
	{
		return NULL;
	} // end if
	
	list_node_t *n = NULL;
	
	while ((n = list_iterator_next(it)))
	{
		if (l->match(n->data, v))
		{
			list_iterator_destroy(it);
			return n;
		} // end if
	} // end while
	
	list_iterator_destroy(it);
	return NULL;
} // end list_find_node()

list_node_t *
list_find_node_at(list_t *l, list_index_t index)
{
	
	list_direction_t d = HEAD;
	
	if (index <= 0)
	{
		if (index == 0)
		{
			return NULL;
		} // end if
		else if (-index > l->len)
		{
			return NULL;
		} // end else if
		else
		{
			d = TAIL;
			index = -index;
		} // end else
	} // end if
	
	printf("index = %d, len = %d\n", index, l->len);
	if  (index <= l->len)
	{
		list_iterator_t *it = list_iterator_create(l, d);
		if (!it)
		{
			return NULL;
		} // end if
		
		list_node_t *n = NULL;
		while (index--)
		{
			n = list_iterator_next(it);
		} // end while
		list_iterator_destroy(it);
		return n;
		
	} // end if
	
	return NULL;
} // end list_find_node_at()

list_len_t
list_get_lenght(list_t *l)
{
	if (!l)
	{
		return -1;
	} // end if
	
	return l->len;
} // end list_get_lenght()

static void 
safe_free(void **pp)
{
	if (pp != NULL && *pp != NULL)
	{
		free(*pp);
		*pp = NULL;
	} // end if
}// end safe_free()
