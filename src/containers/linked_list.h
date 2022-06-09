/**
 * @brief Linked lists in singly and doubly flavors
 * @authors Matheus Xavier <mxavier[AT]neonimp.com>
 */

#pragma once

#ifndef _RTHOST_LL_
#define _RTHOST_LL_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Simple linked list node struct
 */
struct singly_linked_list_node {
  struct singly_linked_list_node *next;
  /// Pointer to the data held by this node
  void *dt_ptr;
};

typedef struct singly_linked_list_node ll_node_t;

struct doubly_linked_list_node {
  struct doubly_linked_list_node *next;
  struct doubly_linked_list_node *prev;
  /// Pointer to the data held by the node
  void *dt_ptr;
};

typedef struct doubly_linked_list_node dll_node_t;

struct linked_list {
  ll_node_t *first;
  ll_node_t *last;
  size_t len;
};

typedef struct linked_list ll_t;

enum LL_ADD_OP {
  PREPEND,
  APPEND,
  INSERT
};

/**
 * @brief Initialize a new singly linked list, you can set all the parameters
 * bellow to create an empty list.
 * @param fval Pointer to the value of the first node.
 * @return A new linked, possibly initialized with a first node.
 */
extern ll_t *ll_new(void *fval);

/**
 * @brief Add a new node created from the parameters to the linked list
 * @note val_sz and val_isz exist as a convenience, they are free to be used
 * in application specific ways.
 * @param self The linked list to change
 * @param val The value to assign the list node
 * @param op Where to insert the node, if not set to INSERT pos is ignored
 * @param pos If op is INSERT where to insert the node in the list
 * @return 0 on success, -1 otherwise
 */
extern int ll_add_node(ll_t *self, void *val, enum LL_ADD_OP op, size_t pos);

#endif /* END _RTHOST_LL_ */
