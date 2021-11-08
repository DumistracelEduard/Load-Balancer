// Copyright 2021 Dumistracel Eduard-Costin
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cdll.h"
#include "utils.h"
#include"Hashtable.h"

doubly_linked_list_t*
dll_create(unsigned int data_size)
{
    doubly_linked_list_t* list;
    list = malloc(sizeof(doubly_linked_list_t));
    DIE(list == NULL, "Error\n");
    list->size = 0;
    list->data_size = data_size;
    return list;
}

dll_node_t*
dll_get_nth_node(doubly_linked_list_t* list, unsigned int n)
{
    unsigned int i;
    dll_node_t* tail = list->head;
    while (n >= list->size) {
        n = n - list->size;
    }
    for (i = 0; i < n; ++i){
         tail = tail->next;
    }
    return tail;
}

void
dll_add_nth_node(doubly_linked_list_t* list, unsigned int n, const void* data)
{
    unsigned int i;
    dll_node_t* node = malloc(sizeof(dll_node_t));
    DIE(node == NULL, "error");
    node->data = malloc(list->data_size);
    memcpy(node->data, data, list->data_size);

    if (n == 0) {
        if (list->size == 0)
        {
            list->head = node;
            node->next = node;
            node->prev = node;
        } else {
            dll_node_t* tail = list->head;
            while (tail->next != list->head) {
                tail = tail->next;
            }
            tail->next = node;
            node->prev = tail;
            list->head->prev = node;
            node->next = list->head;
            list->head = node;
        }
    } else {
        if (n >= list->size) {
            n = list->size;
        }
        if (n == 0) {
            if (list->size == 0) {
                list->head = node;
                node->next = node;
                node->prev = node;
            } else {
                dll_node_t* tail = list->head;
                while (tail->next != list->head) {
                    tail = tail->next;
                }
                tail->next = node;
                node->prev = tail;
                list->head->prev = node;
                node->next = list->head;
                list->head = node;
            }
        } else {
            if (n == list->size) {
                dll_node_t* tail = list->head;
                while (tail->next != list->head) {
                    tail = tail->next;
                }
                tail->next = node;
                node->prev = tail;
                list->head->prev = node;
                node->next = list->head;
            } else {
                dll_node_t *node_before = list->head;
                dll_node_t *node_after;
                for (i = 0; i < n-1; ++i){
                    node_before = node_before->next;
                }
                node_after = node_before->next;
                node_before->next = node;
                node->prev = node_before;
                node->next = node_after;
                node_after->prev = node;
            }
        }
    }
    ++list->size;
}

dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, unsigned int n)
{
    unsigned int i;
    dll_node_t* node = NULL;
    dll_node_t* current = list->head;
    if (list == NULL) {
        return NULL;
    }
    if (n == 0) {
        while (current->next != list->head) {
            current = current->next;
        }
        node = list->head;
        list->head = node->next;
        list->head->prev = current;
        current->next = list->head;
    } else if (n > 0) {
        if (n > list->size) {
            n = list->size -1;
        }
        dll_node_t* next_node;
        for (i = 0; i < n - 1; ++i) {
            current = current->next;
        }
        next_node = current->next->next;
        node = current->next;
        next_node->prev = current;
        current->next = next_node;
    }
    --list->size;
    return node;
}

unsigned int
dll_get_size(doubly_linked_list_t* list)
{
    return list->size;
}

void
dll_free(doubly_linked_list_t** pp_list)
{
    unsigned int i;
    doubly_linked_list_t *list = *pp_list;
    dll_node_t *node = list->head;
    unsigned int n = list->size;
    for (i = 0; i < n; ++i) {
        node = dll_remove_nth_node(list, 0);
        free(((info *)node->data)->key);
        free(((info *)node->data)->value);
        free(node->data);
        free(node);
    }
    free(list);
}

void
dll_print_int_list(doubly_linked_list_t* list)
{
    dll_node_t* current_node = list->head;
    if (list == NULL) {
        printf("Error");
        return;
    }
    while (current_node->next != list->head) {
        printf("%u ", *((int *)current_node->data));
        current_node = current_node->next;
    }
    printf("%u ", *((int *)current_node->data));
    printf("\n");
}

void
dll_print_string_list(doubly_linked_list_t* list)
{
    dll_node_t* current_node = list->head;
    if (list->size == 0) {
        printf("Error");
        return;
    }
    while (current_node->next != list->head) {
        printf("%s ", (char *)current_node->data);
        current_node = current_node->next;
    }
    printf("%s ", (char *)current_node->data);
    printf("\n");
}

void
dll_print_ints_left_circular(dll_node_t* start)
{
    if (start == NULL)
        return;
    dll_node_t* current_node = start;
    while (current_node->prev != start) {
        printf("%d ", *((int *)current_node->data));
        current_node = current_node->prev;
    }
    printf("%d ", *((int *)current_node->data));
    printf("\n");
}

void
dll_print_ints_right_circular(dll_node_t* start)
{
    if (start == NULL)
        return;
    dll_node_t* current_node = start;
    while (current_node->next !=start) {
        printf("%d ", *((int *)current_node->data));
        current_node = current_node->next;
    }
    printf("%d ", *((int *)current_node->data));
    printf("\n");
}
