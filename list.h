#ifndef LIST_H
#define LIST_H

typedef  struct _list_node
{
	struct _list *container;
	struct _list_node *prev;
	struct _list_node *next;
	void *data;
}list_node_t;

typedef void (*data_dtor_func_t)(void *); 
typedef int (*match_func_t)(void *, void *); 
typedef int list_index_t;
typedef int list_len_t;

typedef struct _list
{
	list_node_t *head;
	list_node_t *tail;
	list_len_t len;
	data_dtor_func_t data_dtor;
	match_func_t match;
}list_t;

typedef enum
{
	HEAD,
	TAIL
}list_direction_t;

typedef struct 
{
	list_node_t *next;
	list_direction_t direction;
}list_iterator_t;

list_t * 
list_create(data_dtor_func_t dtor, match_func_t match);

int 
list_destroy(list_t *l);

list_node_t *
list_push_back(list_t *l, list_node_t *n);

list_node_t *
list_pop_back(list_t *l);

int
list_pop_back_and_destroy(list_t *l);

list_node_t *
list_push_front(list_t *l, list_node_t *n);

list_node_t *
list_pop_front(list_t *l);

int
list_pop_front_and_destroy(list_t *l);

list_node_t *
list_remove_node(list_node_t *n);

int 
list_remove_and_destroy_node(list_node_t *n);

list_node_t *
list_insert_before(list_node_t *old, list_node_t *new);

list_node_t *
list_insert_after(list_node_t *old, list_node_t *new);

list_iterator_t *
list_iterator_create(list_t *l, list_direction_t d);

list_node_t *
list_iterator_next(list_iterator_t *it);

void 
list_iterator_destroy(list_iterator_t *it);

list_node_t *
list_node_create(void *data);

int 
list_node_destroy(list_node_t *n);

list_node_t *
list_find_node(list_t *l, void *v);

list_node_t *
list_find_node_at(list_t *l, list_index_t index);

list_len_t
list_get_lenght(list_t *l);

#endif // LIST_H
