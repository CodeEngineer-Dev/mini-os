#include <stddef.h>
#include <stdint.h>

#ifndef LIST_H
#define LIST_H

// This implements a linked list for any type you need a linked list for.

// Type alias called [nodeType]_list_t for a linked list of a certain type.
#define DEFINE_LIST(nodeType) \
typedef struct nodeType##list { \
    struct nodeType * head; \
    struct nodeType * tail; \
    uint32_t size;\
} nodeType##_list_t;

// Makes two new types used to access the next_[nodeType] pointers from a node
#define DEFINE_LINK(nodeType) \
struct nodeType * next##nodeType;\
struct nodeType * prev##nodeType;

// Initializes an empty list by setting its head and tail to an 0 address and its size to zero
#define INITIALIZE_LIST(list) \
    list.head = list.tail = (void *)0;\
    list.size = 0;

// Generates functions for that linked list type
// append goes to the list's tail, makes the next node the inputted node, makes the inputted node's previous node the tail of the list, makes the tail of the list the inputted node, makes the next node of the inputted node null, and increases the list size by 1
// push makes the inputted head's next node the list's head, makes the previous node of it null, makes the head the inputted node, and increases the size of the list by one.
// peek returns the first node of the list
// pop removes and returns the first node of the list
// size returns the size of the linked list
// next returns the node after the node inputted
#define IMPLEMENT_LIST(nodeType) \
void append_##nodeType##_list(nodeType##_list_t * list, struct nodeType * node) {  \
    list->tail->next##nodeType = node;                                       \
    node->prev##nodeType = list->tail;                                       \
    list->tail = node;                                                       \
    node->next##nodeType = NULL;                                             \
    list->size += 1;                                                         \
}                                                                            \
                                                                             \
void push_##nodeType##_list(nodeType##_list_t * list, struct nodeType * node) {    \
    node->next##nodeType = list->head;                                       \
    node->prev##nodeType = NULL;                                             \
    list->head = node;                                                       \
    list->size += 1;                                                         \
}                                                                            \
                                                                             \
struct nodeType * peek_##nodeType##_list(nodeType##_list_t * list) {         \
    return list->head;                                                       \
}                                                                            \
                                                                             \
struct nodeType * pop_##nodeType##_list(nodeType##_list_t * list) {          \
    struct nodeType * res = list->head;                                      \
    list->head = list->head->next##nodeType;                                 \
    list->head->prev##nodeType = NULL;                                                 \
    list->size -= 1;                                                         \
    return res;                                                              \
}                                                                            \
                                                                             \
uint32_t size_##nodeType##_list(nodeType##_list_t * list) {                  \
    return list->size;                                                       \
}                                                                            \
                                                                             \
struct nodeType * next_##nodeType##_list(struct nodeType * node) {           \
    return node->next##nodeType;                                             \
}                                                                            \

#endif
