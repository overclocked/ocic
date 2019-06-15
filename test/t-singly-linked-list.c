/* ------------------------------------------------------------------------- *\
   unit tests for singly linked lists
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "singly-linked-list.h"

/* Unit Tests */
static bool _test_create(bool);
static void _fake_free(void*);
static bool _test_free(bool);
static bool _test_append_item(bool);
static bool _test_prepend_item(bool);
static bool _test_size(bool);
static bool _test_first(bool);
static bool _test_last(bool);
static bool _test_next(bool);
static bool _test_iter(bool);
static void _process_item(void*);
static bool _test_reverse_empty(bool);
static bool _test_reverse_single(bool);
static bool _test_reverse_several(bool);

/* Entry Point */
int test_singly_linked_list( bool );

static bool _test_create(bool quiet)
{
  sll *s = sll_create(NULL);
  if (!s) {
    if (!quiet) printf("ERR: Creating SLL Failed: no object returned.\n");
    return false;
  }
  if (sll_size(s) != 0) {
    if (!quiet) printf("ERR: Creating SLL Failed: expected 0 size.\n");
    return false;
  }
  if (sll_first(s) != NULL) {
    if (!quiet) printf("ERR: Creating SLL Failed: expected null head.\n");
    return false;
  }
  if (sll_last(s) != NULL) {
    if (!quiet) printf("ERR: Creating SLL Failed: expected null tail.\n");
    return false;
  }
  return true;
}

static int free_ctr = 0;
static void _fake_free(void *item) {
  (void) item;
  free_ctr++;
}

static bool _test_free(bool quiet)
{
  sll *s = sll_create(NULL);
  char* i1 = malloc(sizeof(char) * 10);
  char* i2 = malloc(sizeof(char) * 10);
  char* i3 = malloc(sizeof(char) * 10);
  strncpy(i1, "aa", 3);
  strncpy(i2, "bb", 3);
  strncpy(i3, "cc", 3);
  sll_append(s, i1);
  sll_append(s, i2);
  sll_append(s, i3);
  sll_free(s);
  /* Internal memory should still be intact, although how do I verify? */

  free_ctr = 0;
  s = sll_create(&_fake_free);
  sll_append(s, i1);
  sll_append(s, i2);
  sll_append(s, i3);
  sll_free(s);
  if (free_ctr != 3) {
    if (!quiet) printf("ERR: memory release func not called as expected.\n");
    return false;
  }
  /* Internal memory should still be intact because we didn't actually free it. */

  s = sll_create(&free);
  sll_append(s, i1);
  sll_append(s, i2);
  sll_append(s, i3);
  sll_free(s);
  /* Now the memory should really be free, and nothing should SEGV. */
  return true;
}

static bool _test_append_item(bool quiet)
{
  sll *s = sll_create(NULL);
  char* first = "Foo";
  char* second = "Bar";
  char* last   = "Rabbit";
  char* check;
  sll_append(s, first);
  sll_append(s, second);
  sll_append(s, last);
  check = sll_last(s);
  if (check != last) {
    if (!quiet) printf("ERR: Appending to SLL invalid.\n");
    return false;
  }
  return true;
}

static bool _test_prepend_item(bool quiet)
{
  sll *s = sll_create(NULL);
  char* first = "Foo";
  char* second = "Bar";
  char* last   = "Rabbit";
  char* check;
  sll_prepend(s, first);
  sll_prepend(s, second);
  sll_prepend(s, last);
  check = sll_first(s);
  if (check != last) {
    if (!quiet) printf("ERR: Prepending to SLL invalid.\n");
    return false;
  }
  return true;
}

static bool _test_size(bool quiet)
{
  sll *s = sll_create(NULL);
  char* first = "Foo";
  char* second = "Bar";
  char* last   = "Rabbit";
  sll_prepend(s, first);
  sll_append(s, second);
  sll_prepend(s, last);
  if (sll_size(s) != 3) {
    if (!quiet) printf("ERR: Computing SLL Size invalid.\n");
    return false;
  }
  return true;
}

static bool _test_first(bool quiet)
{
  sll *s = sll_create(NULL);
  char* first = "Foo";
  char* second = "Bar";
  char* last   = "Rabbit";
  char* check;
  check = sll_first(s);
  if (check != NULL) {
    if (!quiet) printf("ERR: SLL Empty List first should be null.");
    return false;
  }
  sll_prepend(s, first);
  sll_append(s, second);
  sll_prepend(s, last);
  check = sll_first(s);
  if (check != last) {
    if (!quiet) printf("ERR: SLL - testing first failed.\n");
    return false;
  }
  return true;
}

static bool _test_last(bool quiet)
{
  sll *s = sll_create(NULL);
  char* first = "Foo";
  char* second = "Bar";
  char* last   = "Rabbit";
  char* check;
  check = sll_last(s);
  if (check != NULL) {
    if (!quiet) printf("ERR: SLL - last item in empty list should be null.");
    return false;
  }
  sll_prepend(s, first);
  sll_append(s, second);
  sll_prepend(s, last);
  check = sll_last(s);
  if (check != second) {
    if (!quiet) printf("ERR: SLL - testing last item invalid.\n");
    return false;
  }
  return true;
}

static bool _test_next(bool quiet)
{
  sll *s = sll_create(NULL);
  char* first = "Foo";
  char* second = "Bar";
  char* last   = "Rabbit";
  char* check;
  check = sll_next(s);
  if (check != NULL) {
    if (!quiet) printf("ERR: SLL - next item in empty list should be null.");
    return false;
  }
  sll_prepend(s, first);
  sll_append(s, second);
  sll_prepend(s, last);
  check = sll_first(s);
  if (check != last) {
    if (!quiet) printf("ERR: Testing next (iter1) invalid.\n");
    return false;
  }
  check = sll_next(s);
  if (check != first) {
    if (!quiet) printf("ERR: Testing next (iter2) invalid.\n");
    return false;
  }
  check = sll_next(s);
  if (check != second) {
    if (!quiet) printf("ERR: Testing next (iter3) invalid.\n");
    return false;
  }
  check = sll_next(s);
  if (check != NULL) {
    if (!quiet) printf("ERR: Testing next (iter - last) invalid.\n");
    return false;
  }
  return true;
}

static int iter_errs = 0;
static int iter_count = 0;
static char* expect[] = {"cow", "dog", "foot"};
static void _process_item(void* item) {
  char* check = item;
  if (strcmp(check, expect[iter_count++]) != 0) {
    iter_errs++;
  }
}

static bool _test_iter(bool quiet)
{
  sll *s = sll_create(NULL);
  char* first = "cow";
  char* second = "dog";
  char* last   = "foot";
  iter_errs = 0;
  iter_count = 0;
  sll_iter(s, &_process_item);
  sll_append(s, first);
  sll_append(s, second);
  sll_append(s, last);
  sll_iter(s, &_process_item);
  if (iter_errs > 0) {
    if (!quiet) printf("ERR: SLL - Testing iter; errors reported.\n");
    return false;
  }
  return true;
}

static bool _test_reverse_empty(bool quiet)
{
  sll *s = sll_create(NULL);
  sll_reverse(s);
  if (sll_first(s) != NULL) {
    if (!quiet) printf("ERR: SLL - Reversing empty list failed.\n");
    return false;
  }
  return true;
}

static bool _test_reverse_single(bool quiet)
{
  sll *s = sll_create(NULL);
  char* first = "bean";
  sll_append(s, first);
  sll_reverse(s);
  char* check = sll_first(s);
  if (check != first) {
    if (!quiet) printf("ERR: SLL - Reversing one item list failed.\n");
    return false;
  }
  return true;
}

static bool _test_reverse_several(bool quiet)
{
  sll *s = sll_create(NULL);
  char* first = "foot";
  char* second = "dog";
  char* last   = "cow";
  iter_errs = 0;
  iter_count = 0;
  sll_iter(s, &_process_item);
  sll_append(s, first);
  sll_append(s, second);
  sll_append(s, last);
  sll_reverse(s);
  sll_iter(s, &_process_item);
  if (iter_errs > 0) {
    if (!quiet) printf("ERR: SLL - Testing iter; errors reported.\n");
    return false;
  }
  return true;
}

int test_singly_linked_list(bool quiet)
{
  int errs = 0;
  if (!_test_create(quiet)) errs++;
  if (!_test_free(quiet)) errs++;
  if (!_test_append_item(quiet)) errs++;
  if (!_test_prepend_item(quiet)) errs++;
  if (!_test_size(quiet)) errs++;
  if (!_test_first(quiet)) errs++;
  if (!_test_last(quiet)) errs++;
  if (!_test_next(quiet)) errs++;
  if (!_test_iter(quiet)) errs++;
  if (!_test_reverse_empty(quiet)) errs++;
  if (!_test_reverse_single(quiet)) errs++;
  if (!_test_reverse_several(quiet)) errs++;

  if (!quiet) {
    if (errs) {
      printf("[FAIL] : Singly Linked List\n");
    } else {
      printf("[OK]   : Singly Linked List\n");
    }
  }
  return errs;
}
