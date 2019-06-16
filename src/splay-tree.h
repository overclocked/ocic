#ifndef _SPLAY_TREE_H
#define _SPLAY_TREE_H
/* ------------------------------------------------------------------------- *\
   Splay Tree
     - Tree structure for efficient access of non-random data
     - Prefix: splay
     - Takes key, value inputs, stores value under key,
     - Create with a comparator (required) and a map destructor (optional)
     - The map destructor will be called on freeing the tree, and also
       on the removal of any item. If you don't want the tree to own the
       memory, pass in a NULL destructor.
     - items inserted with duplicate keys will replace the prior one, which
       will also free the memory (if a destructor is provided).
   -------------------------------------------------------------------------
   LICENSE: This program is free software. You can modify it and/or re-
   distribute it under the terms of the Apache 2.0 License. You should have
   received a copy of the Apache 2.0 License along with this program. If
   not, please see: http://www.apache.org/licenses/LICENSE-2.0.txt

   DISCLAIMER: This program is distributed in the hope that it will be
   useful, but without any warrantee; without even the implied warantee
   of fitness for any particular purpose. See the License for more details.
\* ------------------------------------------------------------------------- */

#include <stdint.h>
#include "oc-mem.h"
#include "comparator.h"

typedef struct splay splay;
typedef void(*iter_func)(void *key,void *val);

splay*   splay_create(comparator, map_destructor);
void     splay_free(splay*);

void     splay_put(splay*, void* key, void *val);
void*    splay_get(splay*, void* key);
void     splay_remove(splay*, void* key);
uint32_t splay_count(splay*);
void     splay_iter(splay*, iter_func);

#endif
