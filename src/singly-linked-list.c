#include <stdlib.h>
#include "singly-linked-list.h"

typedef struct sll_node {
  struct sll_node* next;
  void* item;
} sll_node;

struct sll {
  uint32_t size;
  sll_node* head;
  sll_node* tail;
  sll_node* pos;
};

sll_node* _sll_create_node( void* );

sll_node* _sll_create_node( void* item )
{
  sll_node *sn = malloc(sizeof(sll_node));
  sn->next = NULL;
  sn->item = item;
  return sn;
}

sll* sll_create()
{
  sll* s = malloc(sizeof(sll));
  s->size = 0;
  s->head = NULL;
  s->tail = NULL;
  s->pos  = NULL;
  return s;
}

void
sll_append(sll *s, void *item)
{
  sll_node *sn = _sll_create_node(item);
  if (s->tail) {
    s->tail->next = sn;
    s->tail = sn;
    s->size++;
  } else {
    s->head = sn;
    s->pos  = sn;
    s->tail = sn;
    s->size = 1;
  }
}

void
sll_prepend(sll *s, void *item)
{
  sll_node *sn = _sll_create_node(item);
  if (s->head) {
    sn->next = s->head;
    s->head = sn;
    s->pos  = sn;
    s->size++;
  } else {
    s->head = sn;
    s->pos  = sn;
    s->tail = sn;
    s->size = 1;
  }
}

uint32_t
sll_size(sll *s)
{
  return s->size;
}

void
sll_free(sll *s )
{
  s->pos = s->head;
  while (s->pos) {
    s->pos = s->head->next;
    free(s->head);
    s->head = s->pos;
  }
  free(s);
}

void*
sll_first(sll *s)
{
  if (!s->head) return NULL;
  return s->head->item;
}

void*
sll_last(sll *s)
{
  if (!s->tail) return NULL;
  return s->tail->item;
}

void*
sll_next(sll *s)
{
  if (!s->pos) return NULL;
  sll_node *sn = s->pos;
  s->pos = s->pos->next;
  return sn->item;
}

void
sll_iter(sll *s, void(*each_item_func)(void*))
{
  s->pos = s->head;
  while(s->pos) {
    each_item_func(s->pos->item);
    s->pos = s->pos->next;
  }
  s->pos = s->head;
}

void
sll_reverse(sll *s)
{
  if (!s->head) return;
  sll_node *next, *prev = NULL;
  s->pos = s->head;
  while(s->pos) {
    next = s->pos->next;
    s->pos->next = prev;
    prev = s->pos;
    s->pos = next;
  }
  s->tail = s->head;
  s->head = prev;
}
