#ifndef _HASH_H
#define _HASH_H

typedef struct _ht_item_t
{
	char *k;
	void *v;
	struct _ht_t *container;
}ht_item_t;

typedef long ht_size_t;

typedef struct _hash_func_data_t
{
	char *str;
	ht_size_t ht_size;
	int i;
}hash_func_data_t;

typedef ht_size_t (*hash_func_t)(hash_func_data_t *);

typedef void (*data_dtor_func_t)(void *);

typedef ht_size_t (*prime_finder_func_t)(ht_size_t);

typedef struct _ht_t
{
	ht_size_t base_size;
	ht_size_t size;
	ht_size_t count;
	ht_item_t **items;
	data_dtor_func_t data_dtor;
	prime_finder_func_t prime_finder;
	hash_func_t hash_func;
}ht_t;

ht_item_t *
create_ht_item(char *k, void *v);

int
_destroy_ht_item(ht_item_t *i, data_dtor_func_t dtor);

int
destroy_ht_item(ht_item_t *i);

ht_t *
_create_ht(int bs, data_dtor_func_t dtor, prime_finder_func_t pf, hash_func_t hf);

ht_t *
create_ht(data_dtor_func_t dtor);

int
destroy_ht(ht_t *ht);

static hash_func_data_t *
create_hfd(char *str, ht_size_t ht_size, int i);

static int
destroy_hfd(hash_func_data_t *hfd);

static ht_size_t
get_hash_code(char *str, unsigned long pn, ht_size_t ht_size);

static ht_size_t
get_ht_index(hash_func_data_t *hfd);

int
add_to_ht(ht_t *ht, ht_item_t *item, int flags);

void *
search_ht(ht_t *ht, char *k);

int
delete_from_ht(ht_t *ht, char *k);

static ht_size_t
is_prime(ht_size_t x);

static ht_size_t
get_next_prime(ht_size_t x);

static int 
resize_ht(ht_t *ht, ht_size_t new_bs);

static int
resize_ht_up(ht_t *ht);

static int
resize_ht_down(ht_t *ht);

static void 
safe_free(void **pp);
#define sfree(p) safe_free((void**)&(p))

#endif // _HASH_H
