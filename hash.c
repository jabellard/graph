#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "hash.h"

static const ht_size_t MIN_BASE_SIZE = 53;

static const ht_size_t PRIME_NUM1 = 523;

static const ht_size_t PRIME_NUM2 = 619;

ht_item_t DELETED_ITEM = {NULL, NULL, NULL};

ht_item_t *
create_ht_item(char *k, void *v)
{
	if (!k)
	{
		return NULL;
	} // end if
	
	ht_item_t *i = malloc(sizeof(ht_item_t));
	if (!i)
	{
		return NULL;
	} // end if
	
	i->k = strdup(k);
	i->v = v;
	return i;
} // end create_ht_item()

int
_destroy_ht_item(ht_item_t *i, data_dtor_func_t dtor)
{
	if (!i || !dtor)
	{
		return -1;
	} // end if
	
	dtor(i->v);
	
	sfree(i->k);
	sfree(i);
	return 0;
} // end _destroy_ht_item()

int
destroy_ht_item(ht_item_t *i)
{
	if (!i || !i->container)
	{
		return -1;
	} // end if
	
	return _destroy_ht_item(i, i->container->data_dtor);
} // end destroy_ht_item()

ht_t *
_create_ht(int bs, data_dtor_func_t dtor, prime_finder_func_t pf, hash_func_t hf)
{
	if (!dtor)
	{
		return NULL;
	} // end if
	
	ht_t *ht = malloc(sizeof(ht_t));
	if (!ht)
	{
		return NULL;
	} // end if
	
	if (bs < MIN_BASE_SIZE)
	{
		ht->base_size = MIN_BASE_SIZE;
	} // end if
	else
	{
		ht->base_size = bs;
	} // end else
	
	if (!pf)
	{
		ht->prime_finder = get_next_prime;
	} // end if
	else
	{
		ht->prime_finder = pf;
	} // end else
	
	ht->size = ht->prime_finder(ht->base_size);
	
	ht->count = 0;
	
	ht->items = calloc(ht->size, sizeof(ht_item_t *));
	if (!ht->items)
	{
		return NULL;
	} // end if
	
	ht->data_dtor = dtor;
	
	if (hf)
	{
		ht->hash_func = hf;
	} // end if
	else
	{
		ht->hash_func = get_ht_index;
	} // end else
	
	return ht;
} // end _create_ht()

ht_t *
create_ht(data_dtor_func_t dtor)
{
	if (!dtor)
	{
		return NULL;
	} // end if
	
	return _create_ht(MIN_BASE_SIZE, dtor, NULL, NULL);
} // end create_ht()

int
destroy_ht(ht_t *ht)
{
	if (!ht || !ht->items)
	{
		return -1;
	} // end if
	
	int i = 0;
	ht_item_t *curr_item = NULL;
	for (i; i < ht->size; i++)
	{
		curr_item = ht->items[i];
		if (curr_item != NULL && curr_item != &DELETED_ITEM)
		{
			int res = destroy_ht_item(curr_item);
			if (res == -1)
			{
				return -1;
			} // end if
		} // end if
	} // end for
	
	sfree(ht->items);
	sfree(ht);
	return 0;
} // end destroy_ht()

static hash_func_data_t *
create_hfd(char *str, ht_size_t ht_size, int i)
{
	if (!str)
	{
		return NULL;
	} // end if
	
	hash_func_data_t *hfd = malloc(sizeof(hash_func_data_t));
	if (!hfd)
	{
		return NULL;
	} // end if
	
	hfd->str = strdup(str);
	hfd->ht_size = ht_size;
	hfd->i = i;
	return hfd;
} // end create_hfd()

static int
destroy_hfd(hash_func_data_t *hfd)
{
	if (!hfd)
	{
		return -1;
	} // end if
	
	sfree(hfd->str);
	sfree(hfd);
	return 0;
} // end destroy_hfd()

static ht_size_t
get_hash_code(char *str, unsigned long pn, ht_size_t ht_size)
{
	if (!str)
	{
		return -1;
	} // end if
	
	long hash_code = 0;
	const int str_len = strlen(str);
	
	int i = 0;
	for (i; i< str_len; i++)
	{
		hash_code += (long)(pow(pn, str_len - (i + 1)) * str[i]);
		hash_code %= ht_size;
	} // end for
	
	return (ht_size_t)hash_code;
} // end get_hash_code()

static ht_size_t 
get_ht_index(hash_func_data_t *hfd)
{
	if (!hfd || !hfd->str)
	{
		return -1;
	} // end if
	
	const ht_size_t hash_code1 = get_hash_code(hfd->str, PRIME_NUM1, hfd->ht_size);
	if (hash_code1 == -1)
	{
		return -1;
	} // end if
	const ht_size_t hash_code2 = get_hash_code(hfd->str, PRIME_NUM2, hfd->ht_size);
	if (hash_code2 == -1)
	{
		return -1;
	} // end if
	
	return ((hash_code1 + (hfd->i * (hash_code2 + 1))) % hfd->ht_size);
} // end get_ht_index()

int
add_to_ht(ht_t *ht, ht_item_t *item, int flags)
{
	if (!ht || !ht->items || !ht->hash_func || !item)
	{
		return -1;
	} // end if
	
	ht_size_t load = ht->count * 100 / ht->size;
	if (load > 70)
	{
		int res = resize_ht_up(ht);
		if (res == -1)
		{
			return -1;
		} // end if
	} // end if
	
	hash_func_data_t *hfd = create_hfd(item->k, ht->size, 0);
	if (!hfd)
	{
		return -1;
	} // end if
	
	ht_size_t index = ht->hash_func(hfd);
	if (index == -1)
	{
		int res = destroy_hfd(hfd);
		if (res == -1)
		{
			return -1;
		} // end if
		
		return -1;
	}// end if
	
	ht_item_t *curr_item = ht->items[index];
	ht_size_t i = 1;
	while (curr_item != NULL && curr_item != &DELETED_ITEM)
	{
		if (strcmp(curr_item->k, item->k) == 0)
		{
			if (!flags)
			{
				int res = destroy_hfd(hfd);
				if (res == -1)
				{
					return -1;
				} // end if
				
				return -1; 
			} // end if
			else
			{
				int res = destroy_hfd(hfd);
				if (res == -1)
				{
					return -1;
				} // end if
								
				res = destroy_ht_item(curr_item);
				if (res == -1)
				{
					return -1;
				} // end if
				
				item->container = ht;
				ht->items[index] = item;
				return 0;
			} // end else
		} // end if
		
		hfd->i = i;
		index = ht->hash_func(hfd);
		if (index == -1)
		{
			int res = destroy_hfd(hfd);
			if (res == -1)
			{
				return -1;
			} // end if
			
			return -1;
		} // end if
		
		curr_item = ht->items[index];
		i++;	
	} // end while
	
	item->container = ht;
	ht->items[index] = item;
	(ht->count)++;
	return 0;
} // end add_to_ht()

void *
search_ht(ht_t *ht, char *k)
{
	if (!ht || !ht->items || !ht->hash_func || !k)
	{
		return NULL;
	} // end if

	hash_func_data_t *hfd = create_hfd(k, ht->size, 0);
	if (!hfd)
	{
		return NULL;
	} // end if
	
	ht_size_t index = ht->hash_func(hfd);
	if (index == -1)
	{
		int res = destroy_hfd(hfd);
		if (res == -1)
		{
			return NULL;
		} // end if
		
		return NULL;
	}// end if
	
	ht_item_t *curr_item = ht->items[index];
	ht_size_t i = 1;
	while (curr_item != NULL && curr_item != &DELETED_ITEM)
	{
		if (strcmp(curr_item->k, k) == 0)
		{
			int res = destroy_hfd(hfd);
			if (res == -1)
			{
				return NULL;
			} // end if
			
			return curr_item->v;
		} // end if
		
		hfd->i = i;
		index = ht->hash_func(hfd);
		if (index == -1)
		{
			int res = destroy_hfd(hfd);
			if (res == -1)
			{
				return NULL;
			} // end if			
			
			return NULL;
		} // end if
		
		curr_item = ht->items[index];
		i++;	
	} // end while
	
	int res = destroy_hfd(hfd);
	if (res == -1)
	{
		return NULL;
	} // end if
	
	return NULL;
} // end search_ht()

int
delete_from_ht(ht_t *ht, char *k)
{
	if (!ht || !ht->items || !ht->hash_func || !k)
	{
		return -1;
	} // end if
	
	ht_size_t load = ht->count * 100 / ht->size;
	if (load < 10)
	{
		int res = resize_ht_down(ht);
		if (res == -1)
		{
			return -1;
		}
	} // end if
	
	hash_func_data_t *hfd = create_hfd(k, ht->size, 0);
	if (!hfd)
	{
		return -1;
	} // end if
	
	ht_size_t index = ht->hash_func(hfd);
	if (index == -1)
	{
		int res = destroy_hfd(hfd);
		if (res == -1)
		{
			return -1;
		} // end if
		return -1;
	} // end if
	
	ht_item_t *curr_item = ht->items[index];
	ht_size_t i = 1;
	while (curr_item != NULL && curr_item != &DELETED_ITEM)
	{
		if (strcmp(curr_item->k, k) == 0)
		{
			int res = destroy_ht_item(curr_item);
			if (res == -1)
			{
				int res = destroy_hfd(hfd);
				if (res == -1 )
				{
					return -1;
				} // end if
				return -1;
			} // end if
			
			ht->items[index] = &DELETED_ITEM;
			ht->count--;
			return 0;
		} // end if
		
		hfd->i = i;
		index = ht->hash_func(hfd);
		if (index == -1)
		{
			int res = destroy_hfd(hfd);
			if (res == -1)
			{
				return -1;
			} // end if
			return -1;
		} // end if
		
		curr_item = ht->items[index];
		i++;	
	} // end while
	
	int res = destroy_hfd(hfd);
	if (res == -1)
	{
		return -1;
	} // end if
	
	return -1;
} // end delete_from_ht()

static ht_size_t
is_prime(ht_size_t x)
{
	if (x < 2)
	{
		return 0;
	} // end if
	
	if (x < 4)
	{
		return 1;
	} // end if
	
	if (x % 2 == 0)
	{
		return 0;
	} // end if
	
	ht_size_t i = 3;
	for (i; i <= floor(sqrt((double)x)); i += 2)
	{
		if (x % i == 0)
		{
			return 0;
		} // end if
	} // end for
	
	return 1;
} // end is_prime()

static ht_size_t
get_next_prime(ht_size_t x)
{
	while (!is_prime(x))
	{
		x++;
	} // end while
	
	return x;
} // end get_next_prime()

static int 
resize_ht(ht_t *ht, ht_size_t new_bs)
{
	if (!ht)
	{
		return -1;
	} // end if
	
	if (new_bs < MIN_BASE_SIZE)
	{
		return 0;
	} // end if
	
	ht_t *new_ht = _create_ht(new_bs, ht->data_dtor, ht->prime_finder, ht->hash_func);
	if (!new_ht)
	{
		return -1;
	} // end if
	
	ht_size_t i = 0;
	for (i; i < ht->size; i++)
	{
		ht_item_t *curr_item = ht->items[i];
		if (curr_item != NULL && curr_item != &DELETED_ITEM)
		{
			int res = add_to_ht(new_ht, curr_item, 0);
			if (res == -1)
			{
				return -1;
			} // end if
		} // end if
	} // end for
	
	ht_size_t tmp_bs = ht->base_size;
	ht->base_size = new_ht->base_size;
	new_ht->base_size = tmp_bs;
	
	ht_size_t tmp_size = ht->size;
	ht->size = new_ht->size;
	new_ht->size = tmp_size;
	
	ht_size_t tmp_count = ht->count;
	ht->count = new_ht->count;
	new_ht->count = tmp_count;
	
	ht_item_t **tmp_items = ht->items;
	ht->items = new_ht->items;
	new_ht->items = tmp_items;
	
	int res = destroy_ht(new_ht);
	if (res == -1)
	{
		return -1;
	} // end if
	else
	{
		return 0;
	}
} // end resize_ht()

static int
resize_ht_up(ht_t *ht)
{
	if (!ht || ht->base_size == 0)
	{
		return -1;
	} // end if
	
	ht_size_t new_bs = ht->base_size * 2;
	
	return resize_ht(ht, new_bs);
} // end resize_ht_up()

static int
resize_ht_down(ht_t *ht)
{
	if (!ht || ht->base_size == 0)
	{
		return -1;
	} // end if
	
	ht_size_t new_bs = ht->base_size / 2;
	
	return resize_ht(ht, new_bs);
} // end resize_ht_down()

static void safe_free(void **pp)
{
	if (pp != NULL && *pp != NULL)
	{
		free(*pp);
		*pp = NULL;
	} // end if
} // end safe_free()
