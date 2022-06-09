/**
 * @brief Ready to use hashmaps
 * @authors Matheus Xavier <mxavier[AT]neonimp.com>
 */

#pragma once

#ifndef _RTHOST_HASHMAP_
#define _RTHOST_HASHMAP_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../rt_buffer/rt_buffer.h"

struct hashmap_node {
  void *data;
  size_t item_sz;
  size_t data_sz;
  rt_buff_t *key_name;
};

typedef struct hashmap_node hmap_nod_t;

struct hashmap_container {
  hmap_nod_t *nodes;
  size_t node_ct;
  uint32_t curr_load;
  uint32_t load_max;
  uint8_t expand_factor;
};

#endif /* END _RTHOST_HASHMAP_ */
