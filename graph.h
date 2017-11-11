 #ifndef GRAPH_H
#define GRAPH_H

typedef void (*data_dtor_func_t)(void *);

typedef struct _graph_vertex_t
{
	void *data;
	char *key;
	list_t *edges;
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
	size_t size;
}graph_t;

#endif 
