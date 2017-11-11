 #ifndef GRAPH_H
#define GRAPH_H

#include "list.h"
#include "hash.h"

typedef void (*data_dtor_func_t)(void *);

typedef struct _graph_vertex_t
{
	void *data;
	char *key;
	list_t *edges;
	struct _graph_t *container;
}graph_vertex_t;

typedef int graph_edge_weight_t;

typedef struct _graph_edge_t
{
	struct _graph_vertex_t *src;
	struct _graph_vertex_t *dest;
	graph_edge_weight_t weight;
}graph_edge_t;

typedef struct _graph_t
{
	ht_t *vertices;
	data_dtor_func_t vertex_data_dtor;
	size_t size;
}graph_t;

void
graph_edge_dtor(void *data);

int
graph_edge_cmp(void *d1, void *d2);

graph_vertex_t *
graph_vertex_create(void *data, char * key);

void
graph_vertex_destroy(graph_vertex_t *v);

void
graph_vertex_dtor(void *data);

graph_edge_t *
graph_edge_create(graph_vertex_t *src, graph_vertex_t *dest, graph_edge_weight_t weight);

void
graph_edge_destroy(graph_edge_t *e);

graph_t *
graph_create(data_dtor_func_t dtor);

void
graph_destroy(graph_t *g);

int
graph_add_vertex(graph_t *g, graph_vertex_t *v, int flags);

int
graph_has_vertex(graph_t *g, const char *key);

graph_vertex_t *
graph_get_vertex(graph_t *g, const char *key);

const list_t *
graph_get_all_vertices(graph_t *g);

int
graph_remove_vertex(graph_t *g, const char *key);

int
graph_remove_all_vertices(graph_t *g);

int
graph_add_edge(graph_t *g, graph_edge_t *e);

list_node_t *
graph_get_edge_node(graph_t *g, const char *src, const char *dest);

int
graph_has_edge(graph_t *g, const char *src, const char *dest);

graph_edge_t *
graph_get_edge(graph_t *g, const char *src, const char *dest);

const list_t *
graph_vertex_get_all_edges(graph_t *g, const char *key);

const list_t *
graph_vertex_edges_of(graph_t *g, const char *key);

const list_t *
graph_get_all_edges(graph_t *g);

int
graph_remove_edge(graph_t *g, const char *src, const char *dest);

int
graph_remove_all_edges(graph_t *g);

static void 
safe_free(void **pp);
#define sfree(p) safe_free((void**)&(p))

#endif 
