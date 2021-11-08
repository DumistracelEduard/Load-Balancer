// Copyright 2021 Dumistracel Eduard-Costin
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "load_balancer.h"
#include "cdll.h"
#include "utils.h"

struct load_balancer {
	doubly_linked_list_t *list;
    unsigned int size;
};

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

load_balancer* init_load_balancer() {
    load_balancer *hashring = malloc(sizeof(load_balancer));
    DIE(hashring == NULL, "Error\n");

    hashring->list = dll_create(sizeof(server_memory));
    hashring->size = 0;

    return hashring;
}

void loader_store(load_balancer* main, char* key, char* value, int* server_id) {
	dll_node_t* node = main->list->head;

    unsigned int hash_item = hash_function_key(key);
    unsigned count = 0;

    while (count < main->size) {
        if (hash_item < ((server_memory *)(node->data))->server_hash) {
            server_store((server_memory *)node->data, key, value);
            *server_id = ((server_memory *)node->data)->server_tag % 100000;
            return;
        }
        node = node->next;
        count++;
    }
    server_store((server_memory *)main->list->head->data, key, value);
    *server_id = ((server_memory *)main->list->head->data)->server_tag % 100000;
}


char* loader_retrieve(load_balancer* main, char* key, int* server_id) {
	dll_node_t* node = main->list->head;
    unsigned int hash_item = hash_function_key(key);
    unsigned count = 0;

    while (count < main->size) {
        if (hash_item < ((server_memory *)(node->data))->server_hash) {
            server_memory * server = node->data;
            hashtable_t *server_current = server->server_table;

            *server_id = ((server_memory *)node->data)->server_tag % 100000;
            return ht_get(server_current, key);
        }
        node = node->next;
        count++;
    }

    server_memory * server = main->list->head->data;
    hashtable_t *server_current = server->server_table;

    *server_id = ((server_memory *)main->list->head->data)->server_tag % 100000;
    return ht_get(server_current, key);
}

void add_server(server_memory* new_server, load_balancer* main, unsigned *count)
{
    dll_node_t* node_current;
    *count = 0;

    if (main->size == 0) {
        dll_add_nth_node(main->list, *count, new_server);
        main->size++;
        return;
    }

    node_current = main->list->head;
    while (*count < main->size) {
        server_memory *server = node_current->data;
        if (new_server->server_hash < server->server_hash) {
            dll_add_nth_node(main->list, *count, new_server);
            main->size++;
            return;
        }
        *count = *count + 1;
        node_current = node_current->next;
    }

    dll_add_nth_node(main->list, main->size, new_server);
    main->size++;
}

dll_node_t* find_item(load_balancer* main, char* key) {
	dll_node_t* node = main->list->head;

    unsigned int hash_item = hash_function_key(key);
    unsigned count = 0;

    while (count < main->size) {
        if (hash_item < ((server_memory *)(node->data))->server_hash) {
            return node;
        }
        node = node->next;
        count++;
    }

    return main->list->head;
}

void item_redistribution(load_balancer* main, dll_node_t* node)
{
    server_memory* server = node->data;

    for (unsigned i = 0; i < server->server_table->hmax; i++) {
        ll_node_t* node_h = server->server_table->buckets[i]->head;
        while (node_h != NULL) {
            char *key = ((info *)node_h->data)->key;
            dll_node_t* new_node = find_item(main, key);

            if (new_node != node) {
                char *value = ((info *)node_h->data)->value;
                server_store((server_memory*)new_node->data, key, value);
                node_h = node_h->next;
                server_remove(node->data, key);
            } else {
                node_h = node_h->next;
            }
        }
    }
}

void loader_add_server(load_balancer* main, int server_id) {
    unsigned count;

    server_memory* server_cpy1 = init_server_memory();
    server_cpy1->server_tag = server_id;
    server_cpy1->server_hash = hash_function_servers(&server_cpy1->server_tag);
    add_server(server_cpy1, main, &count);

    dll_node_t* node = dll_get_nth_node(main->list, count);
    item_redistribution(main, node->next);

    server_memory* server_cpy2 = init_server_memory();
    server_cpy2->server_tag = 100000 + server_id;
    server_cpy2->server_hash = hash_function_servers(&server_cpy2->server_tag);
    add_server(server_cpy2, main, &count);

    node = dll_get_nth_node(main->list, count);
    item_redistribution(main, node->next);

    server_memory* server_cpy3 = init_server_memory();
    server_cpy3->server_tag = 200000 + server_id;
    server_cpy3->server_hash = hash_function_servers(&server_cpy3->server_tag);
    add_server(server_cpy3, main, &count);

    node = dll_get_nth_node(main->list, count);
    item_redistribution(main, node->next);

    free(server_cpy1);
    free(server_cpy2);
    free(server_cpy3);
}

void free_cpy(load_balancer* main, unsigned id)
{
    dll_node_t* node = main->list->head;

    unsigned count = 0;
    while (count != main->size) {
        if (((server_memory*)node->data)->server_tag == id) {
            node = node->next;

            dll_node_t* curr = dll_remove_nth_node(main->list, count);
            item_redistribution(main, curr);

            free_server_memory(curr->data);
            free(curr);
        } else {
            node = node->next;
        }
        ++count;
    }

    main->size--;
}

void loader_remove_server(load_balancer* main, int server_id)
{
	if (main->size == 0) {
        return;
    }

    unsigned int id1, id2, id3;

    id1 = server_id;
    id2 = 100000 + server_id;
    id3 = 200000 + server_id;

    free_cpy(main, id1);
    free_cpy(main, id2);
    free_cpy(main, id3);
}

void free_load_balancer(load_balancer* main) {
    dll_node_t* node = main->list->head;

    for (unsigned i = 0; i < main->size; ++i) {
        dll_node_t* node_aux = node;
        free_server_memory((server_memory *)node->data);
        node = node->next;
        free(node_aux);
    }

    free(main->list);
    free(main);
}
