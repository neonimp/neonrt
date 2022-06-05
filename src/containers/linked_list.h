/**
 * @brief Linked lists in singly and doubly flavors
 * @authors Matheus Xavier <mxavier[AT]neonimp.com>
 */

#pragma once

#ifndef _RTHOST_STRING_
#define _RTHOST_STRING_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct singly_linked_list_node {
  struct singly_linked_list_node *next;
  size_t dt_len;
  size_t dt_isz;
  void *dt_ptr;
};

typedef struct singly_linked_list_node ll_node_t;

struct doubly_linked_list_node {
  struct doubly_linked_list_node *next;
  struct doubly_linked_list_node *prev;
  size_t dt_len;
  size_t dt_isz;
  void *dt_ptr;
};

typedef struct doubly_linked_list_node dll_node_t;



#endif /* END _RTHOST_STRING_ */
