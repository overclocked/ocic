#ifndef _SINGLY_LINKED_LIST_H
#define _SINGLY_LINKED_LIST_H
/* ------------------------------------------------------------------------- *\
   Singly Linked List
     - Your basic singly linked list.
     - Unsorted
     - Prefix: sll
   Maintains an internal pointer to a "current" item; getting the first
   item resets the current pointer to the head; getting next advances and
   then return the next item. Running the iterator always begins from the
   beginning.      
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

typedef struct sll sll;

/* create and free */
sll*
sll_create( void );

void
sll_free(sll*);

/* add to the end; unsorted */
void
sll_append(sll*, void*);

/* add to the beginning; unsorted */
void
sll_prepend(sll*, void*);

/* get the size of the list */
uint32_t
sll_size(sll*);

/* get the first in the list */
void*
sll_first(sll*);

/* get the next in the list, and advance the pointer */
void*
sll_next(sll*);

/* get the last in the list */
void*
sll_last(sll*);

/* iterate the list, passing each item to the provided function */
void
sll_iter(sll*, void(*each_item_func)(void*));

/* for your software developer interviewing reference. */
void
sll_reverse(sll*);

#endif
