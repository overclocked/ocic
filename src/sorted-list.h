#ifndef _SORTED_LIST_H
#define _SORTED_LIST_H
/* ------------------------------------------------------------------------- *\
   Sorted List
     - Your basic sorted, doubly linked list.
     - Prefix: sorl
   Maintains an internal pointer to a "current" item. Getting first sets
   that pointer to the first item; getting the last sets that pointer to the
   last item. Getting next advances the pointer to the next item down the
   list and returns that item; Getting prev advances the pointer to the
   previous item in the list and returns that. Running a forward or reverse
   iteration will always begin at the beginning or the end, respectively.
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

typedef struct sorl sorl;

/* create:
 *   the user needs to pass in a comparison function that will be used to
 *   keep the list in order. This should return -1 on param 2 less than param 1,
 *   1 on param 2 greater than param 1, and 0 if the two params are equal.
 */
sorl*
sorl_create( int(*)(void*,void*) );

void
sorl_free(sorl*);

/* insert an item */
void
sorl_insert(sorl*, void*);

/* get the size of the list */
uint32_t
sorl_size(sorl*);

/* get the first in the list;
 * sets the current item to the first item;
 */
void*
sorl_first(sorl*);

/* advance the current pointer to the next in the list,
 * return the item at that position.
 */
void*
sorl_next(sorl*);

/* advance the current pointer to the previous in the list,
 * return the item at that position.
 */
void*
sorl_prev(sorl*);

/* set the current pointer to the last item in the list,
 * return the item at that position.
 */
void*
sorl_last(sorl*);

/* iterate the list, passing each item to the provided function */
void
sorl_iter(sorl*, void(*each_item_func)(void*));

/* iterate the list (back to front), passing each to the provided function */
void
sorl_iter_rev(sorl*, void(*each_item_func)(void*));

#endif
