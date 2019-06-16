#ifndef _HASH_MAP_H
#define _HASH_MAP_H
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

#include <stdbool.h>
#include <stdint.h>
#include "oc-mem.h"

typedef struct hmap hmap;

hmap*    hmap_create(map_destructor);
void     hmap_free(hmap*);

void     hmap_put(hmap*, char* key, void *val);
void*    hmap_get(hmap*, char* key);
void     hmap_remove(hmap*, char* key);
uint32_t hmap_count(hmap*);

#endif
