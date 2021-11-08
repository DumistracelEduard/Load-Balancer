// Copyright 2021 Dumistracel Eduard-Costin
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#include "Hashtable.h"

#define MAX_BUCKET_SIZE 64

int
compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

unsigned int
hash_function_int(void *a)
{
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int
hash_function_string(void *a)
{
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	unsigned int i;
	hashtable_t *table;

	table = malloc(sizeof(hashtable_t));
	DIE(table == NULL, "Error\n");

	table->hmax = hmax;
	table->size = 0;
	table->compare_function = compare_function;
	table->hash_function = hash_function;

	table->buckets = malloc(hmax * sizeof(linked_list_t *));
	DIE(table->buckets == NULL, "Error\n");
	for(i = 0; i < hmax; ++i) {
		table->buckets[i] = ll_create(sizeof(struct info));
	}

	return table;
}

ll_node_t *
key_find(linked_list_t* bucket, void *key,
		int (*compare_function)(void*, void*))
{
	ll_node_t *node = bucket->head;

	unsigned int i;

	for (i = 0; i < bucket->size; ++i) {
		if (compare_function(key, ((struct  info*)node->data)->key) == 0) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}
void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	ll_node_t *item2;
	unsigned int index = ht->hash_function(key) % ht->hmax;
	struct info *item = malloc(sizeof(struct info));
	DIE(item == NULL, "Error\n");

	item2 = key_find(ht->buckets[index], key, ht->compare_function);

	if (item2 == NULL) {
		item->key = malloc(key_size);
		DIE(item->key == NULL, "Error\n");
		item->value = malloc(value_size);
		DIE(item->value == NULL, "Error\n");

		memcpy(item->key, key, key_size);
		memcpy(item->value, value, value_size);
		ll_add_nth_node(ht->buckets[index], 0, item);
		++ht->size;
	} else {
		free(((struct info*)item2->data)->value);
		((struct info*)item2->data)->value = malloc(value_size);
		DIE(((struct info*)item2->data)->value == NULL, "Error\n");
		memcpy(((struct info*)item2->data)->value, value, value_size);
	}
	free(item);
}

void *
ht_get(hashtable_t *ht, void *key)
{
	unsigned int index = ht->hash_function(key) % ht->hmax;
	ll_node_t *first;

	for (first = ht->buckets[index]->head; first != NULL; first = first->next) {
		if (ht->compare_function(((struct info*)first->data)->key, key) == 0) {
			return ((struct info*)first->data)->value;
		}
	}
	return NULL;
}

int
ht_has_key(hashtable_t *ht, void *key)
{
	unsigned int index = ht->hash_function(key) % ht->hmax;
	ll_node_t *first = ht->buckets[index]->head;

	for (first = ht->buckets[index]->head; first != NULL; first = first->next) {
		if (ht->compare_function(((struct info*)first->data)->key, key) == 0) {
			return 1;
		}
	}
	return 0;
}


void
ht_remove_entry(hashtable_t *ht, void *key)
{
	unsigned int pos = 0;
	unsigned int index = ht->hash_function(key) % ht->hmax;
	ll_node_t *node, *delete;

	for (node = ht->buckets[index]->head; node; node = node->next) {
		if (ht->compare_function(((struct info*)node->data)->key, key) == 0) {
			free(((struct info*)node->data)->key);
			free(((struct info*)node->data)->value);

			delete = ll_remove_nth_node(ht->buckets[index], pos);

			free(delete->data);
			free(delete);

			--ht->size;
			return;
		}
		++pos;
	}
}

void
ht_free(hashtable_t *ht)
{
	unsigned int i;

	for (i = 0; i < ht->hmax; ++i) {
		ll_node_t* node = NULL;

		while (ll_get_size(ht->buckets[i]) > 0) {
			node = ll_remove_nth_node(ht->buckets[i], 0);
			free(((struct info*)node->data)->key);
			free(((struct info*)node->data)->value);
			free(node->data);
			free(node);
		}
		free(ht->buckets[i]);
	}
	free(ht->buckets);
	free(ht);
}

unsigned int
ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int
ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}
