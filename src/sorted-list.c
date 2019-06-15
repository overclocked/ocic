/* ------------------------------------------------------------------------- *\
   Sorted List
     - Your basic sorted, doubly linked list.
     - Prefix: sorl
   -------------------------------------------------------------------------
   LICENSE: This program is free software. You can modify it and/or re-
   distribute it under the terms of the Apache 2.0 License. You should have
   received a copy of the Apache 2.0 License along with this program. If
   not, please see: http://www.apache.org/licenses/LICENSE-2.0.txt

   DISCLAIMER: This program is distributed in the hope that it will be
   useful, but without any warrantee; without even the implied warantee
   of fitness for any particular purpose. See the License for more details.
\* ------------------------------------------------------------------------- */

#include "sorted-list.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct sorl_node {
  struct sorl_node *prev;
  struct sorl_node *next;
  void      *item;
} sorl_node;

struct sorl {
  sorl_node *head;
  sorl_node *tail;
  sorl_node *curr;
  uint32_t   size;
  comparator cmp;
  object_destructor rel;
};

sorl*
sorl_create( comparator compare, object_destructor release )
{
  sorl *s = malloc(sizeof(sorl));
  s->head = NULL;
  s->tail = NULL;
  s->curr = NULL;
  s->size = 0;
  s->cmp  = compare;
  s->rel  = release;

  return s;
}

void
sorl_free(sorl *s)
{
  s->curr = s->head;
  while (s->curr) {
    s->head = s->curr->next;
    if (s->rel) s->rel(s->curr->item);
    free(s->curr);
    s->curr = s->head;
  }
  free(s);
}

void
sorl_insert(sorl *s, void *item)
{
  bool inserted = false;
  s->curr = s->head;
  sorl_node *sn = malloc(sizeof(sorl_node));
  sn->item = item;
  if (!s->curr) {
    /* Empty list. Start her off! */
    sn->prev = NULL;
    sn->next = NULL;
    s->head  = sn;
    s->tail  = sn;
    s->curr  = sn;
  } else {
    sorl_node *prev = NULL;
    while(s->curr) {
      if (s->cmp(item,s->curr->item) > 0) {
        /* Advance */
        prev = s->curr;
        s->curr = s->curr->next;
      } else {
        /* Insert */
        inserted = true;
        if (!prev) {
          /* beginning of list. */
          sn->next = s->head;
          s->head->prev = sn;
          sn->prev = NULL;
          s->head = sn;
          s->curr = sn;
        } else {
          sn->prev = prev;
          prev->next = sn;
          sn->next = s->curr;
          s->curr->prev = sn;
        }
        break;
      }
    }
    if (!inserted) {
      /* Append. */
      s->tail->next = sn;
      sn->prev = s->tail;
      sn->next = NULL;
      s->tail = sn;
    }
  }
  s->curr = s->head;
  s->size++;
}

void
sorl_remove(sorl *s, void *item)
{
  s->curr = s->head;
  while(s->curr) {
    if (s->curr->item == item) {

      /* Adjust forward pointers. */
      if (s->curr->prev) s->curr->prev->next = s->curr->next;
      else s->head = s->curr->next;

      /* Adjust reverse pointers. */
      if (s->curr->next) s->curr->next->prev = s->curr->prev;
      else s->tail = s->curr->prev;

      free(s->curr);
      s->curr = s->head;
      break;
    }
    s->curr = s->curr->next;
  }
}

/* get the size of the list */
uint32_t
sorl_size(sorl *s)
{
  return s->size;
}

/* get the first in the list */
void*
sorl_first(sorl *s)
{
  if (!s->head) return NULL;
  s->curr = s->head;
  return s->head->item;
}

/* get the next in the list, and advance the pointer */
void*
sorl_next(sorl *s)
{
  if (!s->curr) return NULL;
  if (!s->curr->next) return NULL;
  s->curr = s->curr->next;
  return s->curr->item;
}

void*
sorl_prev(sorl *s)
{
  if (!s->curr) return NULL;
  if (!s->curr->prev) return NULL;
  s->curr = s->curr->prev;
  return s->curr->item;
}

/* get the last in the list */
void*
sorl_last(sorl *s)
{
  if (!s->tail) return NULL;
  s->curr = s->tail;
  return s->tail->item;
}

/* iterate the list, passing each item to the provided function */
void
sorl_iter(sorl *s, void(*each_item)(void*))
{
  s->curr = s->head;
  while (s->curr) {
    each_item(s->curr->item);
    s->curr = s->curr->next;
  }
}

void sorl_iter_rev(sorl *s, void(*each_item)(void*))
{
  s->curr = s->tail;
  while (s->curr) {
    each_item(s->curr->item);
    s->curr = s->curr->prev;
  }
}
