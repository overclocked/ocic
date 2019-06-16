/* ------------------------------------------------------------------------- *\
   Hash Map
     - Basic hash map, requiring string keys.
     - Prefix: hmap
     - May resize.
   -------------------------------------------------------------------------
   LICENSE: This program is free software. You can modify it and/or re-
   distribute it under the terms of the Apache 2.0 License. You should have
   received a copy of the Apache 2.0 License along with this program. If
   not, please see: http://www.apache.org/licenses/LICENSE-2.0.txt

   DISCLAIMER: This program is distributed in the hope that it will be
   useful, but without any warrantee; without even the implied warantee
   of fitness for any particular purpose. See the License for more details.
\* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "hash-map.h"

typedef struct map_node {
  uint64_t hash;
  char * key;
  void* item;
  struct map_node *next;
} map_node;

struct hmap {
  map_node ** nodes;
  uint32_t    map_size;
  uint32_t    item_count;
  uint32_t    collisions;
  map_destructor rel;
};

/* Private declarations. */
static void _free_map_node_list(map_node *, map_destructor);
static uint64_t _hash_string(char * key);

/* Debugging and test accessors. */
uint32_t _hmap_size(hmap*);
uint32_t _hmap_collisions(hmap*);

/* Default numbers */
const unsigned int default_size = 1024;

hmap*
hmap_create(map_destructor release)
{
  hmap *h = malloc(sizeof(hmap));
  memset(h, 0, sizeof(hmap));
  h->map_size = default_size;
  h->nodes = malloc(sizeof(hmap*) * default_size);
  memset(h->nodes, 0, sizeof(hmap*) * default_size);
  h->rel = release;
  return h;
}

void
hmap_free(hmap *h)
{
  if (h->rel) {
    for (uint32_t i = 0; i < h->map_size; i++) {
      if (h->nodes[i]) _free_map_node_list(h->nodes[i], h->rel);
    }
  }
  free(h);
}

void
hmap_put(hmap *h, char* key, void *val)
{
  map_node *node = malloc(sizeof(map_node));
  node->key = key;
  node->item = val;
  node->next = NULL;
  node->hash = _hash_string(key);
  uint64_t idx  = node->hash & (uint64_t)(h->map_size - 1); /* Modulo. */
  if (!h->nodes[idx]) {
    h->nodes[idx] = node;
  } else {
    h->collisions++;
    /* TODO: test and expand on collision threshold */
    node->next = h->nodes[idx];
    h->nodes[idx] = node;
  }
  h->item_count++;
  return;
}

void*
hmap_get(hmap *h, char* key)
{
    map_node *n;
    uint64_t hash = _hash_string(key);
    uint32_t idx = hash & (h->map_size - 1);
    if (!h->nodes[idx]) return NULL;
    for (n = h->nodes[idx]; n; n = n->next) {
      if (n->hash == hash && strcmp(key, n->key) == 0) {
        return n->item;
      }
    }
    return NULL;
}

void
hmap_remove(hmap *h, char* key)
{
  map_node *n, *prev;
  uint64_t hash = _hash_string(key);
  uint32_t idx = hash & (h->map_size - 1);
  if (!h->nodes[idx]) return;
  n = h->nodes[idx];
  prev = NULL;
  while (n) {
    if (n->hash == hash && strcmp(key, n->key) == 0) {
      if (prev) {
        prev->next = n->next;
      } else {
        h->nodes[idx] = n->next;
      }
      free(n);
      h->item_count--;
      return;
    }
    prev = n;
    n = n->next;
  }
  return;
}

uint32_t
hmap_count(hmap *h)
{
  return h->item_count;
}

/* ------------------------------------------------------------------------- *\
   private functions.
\* ------------------------------------------------------------------------- */

static void _free_map_node_list(map_node *node, map_destructor r)
{
  if (!node) return;
  if (node->next) {
    _free_map_node_list(node->next, r);
  }
  if (r) r((void*)node->key, node->item);
  free(node);
}

#define MAGIC_PRIME 97
/*
 * After trying many approaches and several different data sets, this
 * algorithm, which is far simpler than many of the ones I took out of
 * books, performs best on small to middling strings ... ie., typical
 * keys into maps.
 */
static uint64_t _hash_string(char * key)
{
  int sz = strlen(key);
  uint64_t hash = (key[sz-1] & 15) * MAGIC_PRIME;

  for(int pos = 0; pos < sz; pos++) {
    hash = ((hash << 5) + hash) ^ key[pos];
  }
  hash += ((key[sz-1] * 7) + (key[sz - 1] & 15));
  return hash;
}

uint32_t _hmap_size(hmap *h)
{
  return h->map_size;
}

uint32_t _hmap_collisions(hmap *h)
{
  return h->collisions;
}
