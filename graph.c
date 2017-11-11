#include <stdlib.h>
#include <string.h>
#include "graph.h"

extern ht_item_t DELETED_ITEM;

void
graph_edge_dtor(void *data)
{
	return graph_edge_destroy((graph_edge_t *)data);
} // end edge_dtor()


int
graph_edge_cmp(void *d1, void *d2)
{
	if (!d1 || !(((graph_edge_t *)d1)->dest) || !d2)
	{
		return 0;
	} // end if

	if (strcmp(((graph_edge_t *)d1)->dest->key, (const char *)d2) == 0)
	{
		return 1;
	} // end if
	
	return 0;	
} // end edge_cmp()

graph_vertex_t *
graph_vertex_create(void *data, char * key)
{
	if (!key)
	{
		return NULL;
	} // end if
	
	graph_vertex_t *v = malloc(sizeof(graph_vertex_t));
	if (!v)
	{
		return NULL;
	} // end if
	
	v->data = data;
	v->key = strdup(key);
	v->edges = create_list(graph_edge_dtor, graph_edge_cmp);
	if (!v->edges)
	{
		sfree(v);
		return NULL;
	} // end if
	
	v->container = NULL;
	
	return v;
} // end graph_vertex_create()

void
graph_vertex_destroy(graph_vertex_t *v)
{
	if (!v)
	{
		return;
	} // end if
	
	if (v->data && v->container && v->container->vertex_data_dtor)
	{
		v->container->vertex_data_dtor(v->data);
	} // end if
	
	if (!v->container)
	{
		sfree(v->key);
	} // end if
	
	destroy_list(v->edges);
	
	sfree(v);
	
	return;
} // end graph_vertex_destroy()

void
graph_vertex_dtor(void *data)
{
	return graph_vertex_destroy((graph_vertex_t *)data);
} // end graph_vertex_dtor()

graph_edge_t *
graph_edge_create(graph_vertex_t *src, graph_vertex_t *dest, graph_edge_weight_t weight)
{
	if (!src || !dest)
	{
		return NULL;
	} // end if
	
	graph_edge_t *e = malloc(sizeof(graph_edge_t));
	if (!e)
	{
		return NULL;
	} // end if
	
	e->src = src;
	e->dest = dest;
	e->weight = weight;
	
	return e;
} // end graph_edge_create()

void
graph_edge_destroy(graph_edge_t *e)
{
	sfree(e);
	return;
} // end graph_edge_destroy()


graph_t *
graph_create(data_dtor_func_t dtor)
{
	if (!dtor)
	{
		return NULL;
	} // end if
	
	graph_t *g = malloc(sizeof(graph_t));
	if (!g)
	{
		return NULL;
	} // end if
	
	g->vertices = create_ht(graph_vertex_dtor);
	if (!g->vertices)
	{
		sfree(g);
		return NULL;
	} // end if
	
	g->vertex_data_dtor = dtor;
	
	g->size = 0;
	
	return g;
} // end graph_create()

void
graph_destroy(graph_t *g)
{
	if (!g)
	{
		return NULL;
	} // end if
	
	if (g->vertices)
	{
		destroy_ht(g->vertices);
	} // end if
	
	sfree(g);
	
	return;
} // end graph_destroy()

int
graph_add_vertex(graph_t *g, graph_vertex_t *v, int flags)
{
	if (!g || !g->vertices || !v)
	{
		return -1;
	} // end if
	
	
	ht_item_t *i = create_ht_item(v->key, (void *)v);
	if (!i)
	{
		return -1;
	} // end if
	
	sfree(v->key);
	v->key = i->k;
	
	int res = add_to_ht(g->vertices, i, flags);
	if (res == -1)
	{
		
		sfree(i);
		return -1;
	} // end if
	
	v->container = g;
	(g->size)++;
	
	return 0;
} // end graph_add_vertex()

int
graph_has_vertex(graph_t *g, const char *key)
{
	if (!g || !g->vertices)
	{
		return 0;
	} // end if
	
	void *res = search_ht(g->vertices, key);
	if (!res)
	{
		return 0;
	} // end if
	
	return 1;
} // end graph_has_vertex()

graph_vertex_t *
graph_get_vertex(graph_t *g, const char *key)
{
	if (!g || !g->vertices)
	{
		return NULL;
	} // end if
	
	return (graph_vertex_t *)search_ht(g->vertices, key);
} // end graph_get_vertex()

const list_t *
graph_get_all_vertices(graph_t *g)
{
	return NULL;
} // end graph_get_all_vertices()

int
graph_remove_vertex(graph_t *g, const char *key)
{
	if (!g || !g->vertices || !g->vertices->items || !key)
	{
		return -1;
	} // end if
	
	size_t i = 0;
	ht_item_t *curr_item = NULL;
	for (i; i < g->vertices->size; i++)
	{
		curr_item = g->vertices->items[i];
		if (curr_item != NULL && curr_item != &DELETED_ITEM)
		{
			list_node_t *n = find_node_by_value(((graph_vertex_t *)curr_item)->edges, key);
			if (n)
			{
				int res = remove_and_destroy_list_node(n);
				if (res == -1)
				{
					return -1;
				} // end if
			} // end if
		} // end if
	} // end for
	
	int res = delete_from_ht(g->vertices, key);
	if (res == -1)
	{
		return -1;
	} // end if
	
	(g->size)--;
	return 0;
} // end graph_remove_vertex()

int
graph_remove_all_vertices(graph_t *g)
{
	if (!g || !g->vertices || !g->vertices->items)
	{
		return -1;
	} // end if
	
	size_t i = 0;
	ht_item_t *curr_item = NULL;
	for (i; i < g->vertices->size; i++)
	{
		curr_item = g->vertices->items[i];
		if (curr_item !=NULL && curr_item != &DELETED_ITEM)
		{
			int res = destroy_ht_item(curr_item);
			if (res == -1)
			{
				return -1;
			} // end if
		} // end if
	} // end for
	
	g->size = 0;
	return 0;
} // end graph_remove_all_vertices()

int
graph_add_edge(graph_t *g, graph_edge_t *e)
{
	if (!g || !e || !e->src || !e->dest)
	{
		return -1;
	} // end if
	
	graph_vertex_t *r1 = graph_get_vertex(g, e->src->key);
	graph_vertex_t *r2 = graph_get_vertex(g, e->dest->key);
	
	if (!r1 || !r2)
	{
		return -1;
	} // end if
	
	list_node_t *n = find_node_by_value(r1->edges, (void *)e->dest->key);
	
	if (n)
	{
		return -1;
	} // end if
	
	list_node_t *new_node = create_list_node((void *)e);
	if (!new_node)
	{
		return -1;
	} // end if
	
	list_node_t *res = add_to_end(r1->edges, new_node);
	if (!res)
	{
		sfree(new_node);
		return -1;
	} // end if
	
	return 0;
} // end graph_add_edge()

list_node_t *
graph_get_edge_node(graph_t *g, const char *src, const char *dest)
{
	if (!g || !src || !dest)
	{
		return NULL;
	} // end if
	
	graph_vertex_t *r1 = graph_get_vertex(g, src);
	graph_vertex_t *r2 = graph_get_vertex(g, dest);
	
	if (!r1 || !r2)
	{
		return NULL;
	} // end if

	return find_node_by_value(r1->edges, (void *)dest);
} // end graph_get_edge_node()

int
graph_has_edge(graph_t *g, const char *src, const char *dest)
{
	list_node_t *n = graph_get_edge_node(g, src, dest);
	
	if (!n)
	{
		return 0;
	} // end if
	
	return 1;
} // end graph_has_edge()

graph_edge_t *
graph_get_edge(graph_t *g, const char *src, const char *dest)
{
	list_node_t *n = graph_get_edge_node(g, src, dest);
	
	if (!n)
	{
		return NULL;
	} // end if
	
	return (graph_edge_t *)n->data;
} // end graph_get_edge()

const list_t *
graph_vertex_get_all_edges(graph_t *g, const char *key)
{
	graph_vertex_t *v = graph_get_vertex(g, key);
	
	if (!v)
	{
		return NULL;
	} // end if
	
	return v->edges;
} // end graph_vertex_get_all_edges()

const list_t *
graph_vertex_edges_of(graph_t *g, const char *key)
{
	return NULL;
} // end graph_vertex_edges_of()

const list_t *
graph_get_all_edges(graph_t *g)
{
	return NULL;
} // end graph_get_all_edges()

int
graph_remove_edge(graph_t *g, const char *src, const char *dest)
{
	list_node_t *n = graph_get_edge_node(g, src, dest);
	
	return remove_and_destroy_list_node(n);
} // end graph_remove_edge()

int
graph_remove_all_edges(graph_t *g)
{
	if (!g || !g->vertices || !g->vertices->items)
	{
		return -1;
	} // end if
	
	size_t i = 0;
	ht_item_t *curr_item = NULL;
	for (i; i < g->vertices->size; i++)
	{
		curr_item = g->vertices->items[i];
		if (curr_item != NULL && curr_item != &DELETED_ITEM)
		{
			list_t *curr_list = ((graph_vertex_t *)curr_item->v)->edges;
			list_node_t *curr_node = curr_list->head;
			list_node_t *next_node = NULL;
			
			while((curr_list->len)--)
			{
				next_node = curr_node->next;
				
				int res = destroy_list_node(curr_node);
				if (res == -1)
				{
					return -1;
				} // end if
				
				curr_node = next_node;
			} // end while
		} // end if
	} // end for
	
	return 0;
} // end graph_remove_all_edges()

static void 
safe_free(void **pp)
{
	if (pp != NULL && *pp != NULL)
	{
		free(*pp);
		*pp = NULL;
	} // end if
}// end safe_free()
