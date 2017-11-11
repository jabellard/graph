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
create_list(data_dtor_func_t dtor, match_func_t match);

int 
destroy_list(list_t *l);

list_node_t *
add_to_end(list_t *l, list_node_t *n);

list_node_t *
remove_from_end(list_t *l);

int
remove_from_end_and_destroy(list_t *l);

list_node_t *
add_to_start(list_t *l, list_node_t *n);

list_node_t *
remove_from_start(list_t *l);

int
remove_from_start_and_destroy(list_t *l);

list_node_t *
remove_list_node(list_node_t *n);

int 
remove_and_destroy_list_node(list_node_t *n);

list_node_t *
insert_before_node(list_node_t *old, list_node_t *new);

list_node_t *
insert_after_node(list_node_t *old, list_node_t *new);

list_iterator_t *
create_list_iterator(list_t *l, list_direction_t d);

list_node_t *
list_iterator_next(list_iterator_t *it);

void 
destroy_list_iterator(list_iterator_t *it);

list_node_t *
create_list_node(void *data);

int 
destroy_list_node(list_node_t *n);

list_node_t *
find_node_by_value(list_t *l, void *v);

list_node_t *
find_node_at_index(list_t *l, list_index_t index);

list_len_t
get_list_lenght(list_t *l);

static void 
safe_free(void **pp);
#define sfree(p) safe_free((void**)&(p))

#endif // LIST_H
