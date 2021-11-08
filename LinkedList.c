// Copyright 2021 Dumistracel Eduard-Costin
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"
#include "utils.h"

linked_list_t* ll_create(unsigned int data_size)
{
    linked_list_t* list;

    list = malloc(sizeof(linked_list_t));
    DIE(list == NULL, "error");

    list->size = 0;
    list->data_size = data_size;
    list->head = NULL;

    return list;
}


void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (list == NULL) {
        return;
    }

    if (n > list->size) {
        n = list->size;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
    DIE(new_node == NULL, "new_node malloc");
    new_node->data = malloc(list->data_size);
    DIE(new_node->data == NULL, "new_node->data malloc");
    memcpy(new_node->data, new_data, list->data_size);

    new_node->next = curr;
    if (prev == NULL) {
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}


ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    unsigned int i;
    ll_node_t *node;
    ll_node_t *current = list->head;
    if (list == NULL || list->head == NULL){
        return NULL;
    }
    if (n == 0){
        node = list->head;
        list->head = node->next;
        --list->size;
        return node;
    } else if (n > 0) {
        if (n > list->size - 1){
            n = list->size - 1;
        }
        for (i = 0; i < n - 1; ++i) {
            current = current->next;
        }
        node = current->next;
        current->next = node->next;
        --list->size;
        return node;
    } else {
        printf("error\n");
        return NULL;
    }
}

unsigned int
ll_get_size(linked_list_t* list)
{
    if (list == NULL) {
        return -1;
    }

    return list->size;
}

void ll_free(linked_list_t** pp_list)
{
    int i;

    linked_list_t *list = *pp_list;
    ll_node_t *node = list->head;

    int n = list->size;

    for (i = 0; i < n; ++i) {
        node = ll_remove_nth_node(list, 0);
        free(node->data);
        free(node);
    }

    free(list);
}

void
ll_print_int(linked_list_t* list)
{
    ll_node_t *current_node = list->head;
    while (current_node != NULL)
    {
        printf("%d ", *((int *)current_node->data));
        current_node = current_node->next;
    }
    printf("\n");
}

void
ll_print_string(linked_list_t* list)
{
    ll_node_t *current_node = list->head;

    while (current_node != NULL) {
        printf("%s ", (char *)current_node->data);
        current_node = current_node->next;
    }

    printf("\n");
}
