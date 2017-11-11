#include <stdlib.c>
#include "graph.h"

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
	v->key = key; // create and destroy???
	v->edges = create_list(edge_tdor, edge_cmp);
	if (!v->edges)
	{
		sfree(v);
		return NULL;
	} // end if
	
	return v;
} // end graph_vertex_create()

void
graph_vertex_destroy(graph_vertex_t *v)
{
	if (!v || !v->data_dtor)
	{
		return;
	} // end if
	
	v->data_dtor(v->data);
	
	// destroy key???
	
	destroy_list(v->edges);
	
	sfree(v);
	
	return;
} // end if graph_vertex_destroy()

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
	
	g->vertices = create_ht(dtor);
	if (!g->vertices)
	{
		sfree(g);
		return NULL;
	} // end if
	
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
graph_add_vertex(graph_t *g, graph_vertex_t *v)
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
	
	int res = add_to_ht(g->vertices, i, 0);
	if (res == -1)
	{
		return -1;
	} // end if
	
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

} // end graph_get_all_vertices()

int
graph_remove_vertex(graph_t *g, const char *key)
{
	if (!g || !g->vertices || !g->vertices->items || !key)
	{
		return -1;
	} // end if
	
	int res = delete_from_ht(g->vertices, key);
	if (res == -1)
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
				remove_and_destroy_list_node(n);
			} // end if
		} // end if
	} // end for
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
			destroy_ht_item(curr_item);
		} // end if
	} // end for
	
	return 0;
} // end graph_remove_all_vertices()

int
graph_add_edge(graph_t *g, graph_edge_t *e)
{

} // end graph_add_edge()

int
graph_has_edge(graph_t *g, const char *src, const char *dest)
{

} // end graph_has_edge()

graph_edge_t *
graph_get_edge(graph_t *g, const char *key)
{

} // end graph_get_edge()

const list_t *
graph_vertex_get_all_edges(graph_t *g, const char *key)
{

} // end graph_vertex_get_all_edges()

const list_t *
graph_vertex_edges_of(graph_t *g, const char *key)
{

} // end graph_vertex_edges_of()

const list_t *
graph_get_all_edges(graph_t *g)
{

} // end graph_get_all_edges()

int
graph_remove_edge(graph_t *g, const char *src, const char *dest)
{

} // end graph_remove_edge()

int
graph_remove_all_edges(graph_t *g)
{

} // end graph_remove_all_edges()
