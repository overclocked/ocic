/* ------------------------------------------------------------------------- *\
   unit tests for sorted lists
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

#include "sorted-list.h"

/* Unit Tests */
static bool _test_sorl_create(bool);
static bool _test_sorl_insert_item(bool);
static bool _test_sorl_remove(bool);
static bool _test_sorl_size(bool);
static bool _test_sorl_first(bool);
static bool _test_sorl_last(bool);
static bool _test_sorl_next(bool);
static bool _test_sorl_prev(bool);
static bool _test_sorl_iter(bool);
static bool _test_sorl_rev_iter(bool);
static void _process_sorl_item(void*);
static void _process_sorl_item_rev(void*);
static int  _sorl_compare(void*, void*);

/* Entry Point */
int test_sorted_list( bool );

static int _sorl_compare(void *one, void *two)
{
  const char* a = one;
  const char* b = two;

  return strcmp(a,b);
}


static bool _test_sorl_create(bool quiet)
{
  sorl *s = sorl_create(&_sorl_compare);
  if (!s) {
    if (!quiet) printf("ERR: Creating SORL Failed: no object returned.\n");
    return false;
  }
  if (sorl_size(s) != 0) {
    if (!quiet) printf("ERR: Creating SORL Failed: expected 0 size.\n");
    return false;
  }
  if (sorl_first(s) != NULL) {
    if (!quiet) printf("ERR: Creating SORL Failed: expected null head.\n");
    return false;
  }
  if (sorl_last(s) != NULL) {
    if (!quiet) printf("ERR: Creating SORL Failed: expected null tail.\n");
    return false;
  }
  return true;
}

static bool _test_sorl_insert_item(bool quiet)
{
  sorl *s = sorl_create(&_sorl_compare);
  char* it_1  = "aa";
  char* it_2  = "bb";
  char* it_3  = "cc";
  char* check;
  sorl_insert(s, it_1);
  sorl_insert(s, it_3); /* Expect to be last. */
  sorl_insert(s, it_2);
  check = sorl_last(s);
  if (check != it_3) {
    if (!quiet) printf("ERR: inserting to SORL invalid. %s\n", check);
    return false;
  }
  return true;
}

static bool _test_sorl_remove(bool quiet)
{
  sorl *s = sorl_create(&_sorl_compare);
  char* it_1  = "aa";
  char* it_2  = "bb";
  char* it_3  = "cc";
  char* check;
  /* Remove on an empty list should be fine. */
  sorl_remove(s, it_1);

  /* Add some items. */
  sorl_insert(s, it_1);
  sorl_insert(s, it_3);
  sorl_insert(s, it_2);

  /* Remove and validate */

  /* Remove first: */
  sorl_remove(s, it_1);
  check = sorl_first(s);
  if (check != it_2) {
    if (!quiet) printf("ERR: removing first item problem.");
    return false;
  }

  sorl_remove(s, it_3);
  check = sorl_last(s);
  if (check != it_2) {
    if (!quiet) printf("ERR: removing last item problem.");
    return false;
  }

  sorl_remove(s, it_2);
  check = sorl_first(s);
  if (check != NULL) {
    if (!quiet) printf("ERR: removing only item problem.");
    return false;
  }

  return true;
}


static bool _test_sorl_size(bool quiet)
{
  sorl *s = sorl_create(&_sorl_compare);
  char* first = "Foo";
  char* second = "Bar";
  char* last   = "Rabbit";
  sorl_insert(s, first);
  sorl_insert(s, second);
  sorl_insert(s, last);
  if (sorl_size(s) != 3) {
    if (!quiet) printf("ERR: Computing sorl Size invalid.\n");
    return false;
  }
  return true;
}

static bool _test_sorl_first(bool quiet)
{
  sorl *s = sorl_create(&_sorl_compare);
  char* it_1  = "aa";
  char* it_2  = "bb";
  char* it_3  = "cc";
  char* it_4  = "dd";
  char* it_5  = "cc";
  char* it_6  = "ff";
  char* check;
  sorl_insert(s, it_4);
  sorl_insert(s, it_3);
  sorl_insert(s, it_2);
  sorl_insert(s, it_5);
  sorl_insert(s, it_1); /* expect first */
  sorl_insert(s, it_6);
  check = sorl_first(s);
  if (check != it_1) {
    if (!quiet) printf("ERR: ordering or getting first item to SORL.\n");
    return false;
  }
  return true;
}

static bool _test_sorl_last(bool quiet)
{
  sorl *s = sorl_create(&_sorl_compare);
  char* it_1  = "aa";
  char* it_2  = "bb";
  char* it_3  = "cc";
  char* it_4  = "dd";
  char* it_5  = "cc";
  char* it_6  = "ff";
  char* check;
  sorl_insert(s, it_4);
  sorl_insert(s, it_3);
  sorl_insert(s, it_6); /* expect last */
  sorl_insert(s, it_2);
  sorl_insert(s, it_5);
  sorl_insert(s, it_1);
  check = sorl_last(s);
  if (check != it_6) {
    if (!quiet) printf("ERR: ordering or getting last item to SORL.\n");
    return false;
  }
  return true;
}

static bool _test_sorl_next(bool quiet)
{
  sorl *s = sorl_create(&_sorl_compare);
  char* it_1  = "aa";
  char* it_2  = "bb";
  char* it_3  = "cc";
  char* it_4  = "dd";
  char* it_5  = "cc";
  char* it_6  = "ff";
  char* check;
  sorl_insert(s, it_4);
  sorl_insert(s, it_3);
  sorl_insert(s, it_6);
  sorl_insert(s, it_2);
  sorl_insert(s, it_5);
  sorl_insert(s, it_1);
  check = sorl_first(s);
  if (check != it_1) {
    if (!quiet) printf("ERR: next item test to SORL. (1: %s)\n", check);
    return false;
  }
  check = sorl_next(s);
  if (check != it_2) {
    if (!quiet) printf("ERR: next item test to SORL. (2: %s)\n", check);
    return false;
  }
  check = sorl_next(s);
  if (check != it_5) {
    if (!quiet) printf("ERR: next item test to SORL. (3: %s)\n", check);
    return false;
  }
  check = sorl_next(s);
  if (check != it_3) {
    if (!quiet) printf("ERR: next item test to SORL. (4: %s)\n", check);
    return false;
  }
  check = sorl_next(s);
  if (check != it_4) {
    if (!quiet) printf("ERR: next item test to SORL. (5: %s)\n", check);
    return false;
  }
  check = sorl_next(s);
  if (check != it_6) {
    if (!quiet) printf("ERR: next item test to SORL. (6: %s)\n", check);
    return false;
  }
  check = sorl_next(s);
  if (check != NULL) {
    if (!quiet) printf("ERR: next item test to SORL. (7: %s)\n", check);
    return false;
  }
  return true;
}

static bool _test_sorl_prev(bool quiet)
{
  sorl *s = sorl_create(&_sorl_compare);
  char* it_1  = "aa";
  char* it_2  = "bb";
  char* it_3  = "cc";
  char* it_4  = "dd";
  char* it_5  = "cc";
  char* it_6  = "ff";
  char* check;
  sorl_insert(s, it_4);
  sorl_insert(s, it_3);
  sorl_insert(s, it_6);
  sorl_insert(s, it_2);
  sorl_insert(s, it_5);
  sorl_insert(s, it_1);
  check = sorl_last(s);
  if (check != it_6) {
    if (!quiet) printf("ERR: prev item test to SORL. (1: %s)\n", check);
    return false;
  }
  check = sorl_prev(s);
  if (check != it_4) {
    if (!quiet) printf("ERR: prev item test to SORL. (2: %s)\n", check);
    return false;
  }
  check = sorl_prev(s);
  if (check != it_3) {
    if (!quiet) printf("ERR: prev item test to SORL. (3: %s)\n", check);
    return false;
  }
  check = sorl_prev(s);
  if (check != it_5) {
    if (!quiet) printf("ERR: prev item test to SORL. (4: %s)\n", check);
    return false;
  }
  check = sorl_prev(s);
  if (check != it_2) {
    if (!quiet) printf("ERR: prev item test to SORL. (5: %s)\n", check);
    return false;
  }
  check = sorl_prev(s);
  if (check != it_1) {
    if (!quiet) printf("ERR: prev item test to SORL. (6: %s)\n", check);
    return false;
  }
  check = sorl_prev(s);
  if (check != NULL) {
    if (!quiet) printf("ERR: prev item test to SORL. (7: %s)\n", check);
    return false;
  }
  return true;
}

static int iter_errs = 0;
static int iter_count = 0;
static int expect[] = {10, 20, 30, 40, 50, 60};
static int revexp[] = {60, 50, 40, 30, 20, 10};
static void _process_sorl_item(void* item) {
  int check = *(int*)item;
  if (expect[iter_count++] != check) {
    iter_errs++;
  }
}
static void _process_sorl_item_rev(void* item) {
  int check = *(int*)item;
  if (revexp[iter_count++] != check) {
    iter_errs++;
  }
}

static bool _test_sorl_iter(bool quiet)
{
  sorl *s = sorl_create(&_sorl_compare);
  int i1 = 10;
  int i2 = 20;
  int i3 = 30;
  int i4 = 40;
  int i5 = 50;
  int i6 = 60;
  iter_errs = 0;
  iter_count = 0;
  /* Empty list should generate no errors. Should be no-op. */
  sorl_iter(s, &_process_sorl_item);
  /* Order of inserts should not matter. */
  sorl_insert(s, &i2);
  sorl_insert(s, &i4);
  sorl_insert(s, &i6);
  sorl_insert(s, &i5);
  sorl_insert(s, &i3);
  sorl_insert(s, &i1);
  sorl_iter(s, &_process_sorl_item);
  if (iter_errs > 0) {
    if (!quiet) printf("ERR: sorl - Testing iter; errors reported.\n");
    return false;
  }
  return true;
}

static bool _test_sorl_rev_iter(bool quiet)
{
  sorl *s = sorl_create(&_sorl_compare);
  int i1 = 10;
  int i2 = 20;
  int i3 = 30;
  int i4 = 40;
  int i5 = 50;
  int i6 = 60;
  iter_errs = 0;
  iter_count = 0;
  /* Empty list should generate no errors. Should be no-op. */
  sorl_iter_rev(s, &_process_sorl_item);
  /* Order of inserts should not matter. */
  sorl_insert(s, &i3);
  sorl_insert(s, &i2);
  sorl_insert(s, &i5);
  sorl_insert(s, &i1);
  sorl_insert(s, &i4);
  sorl_insert(s, &i6);
  sorl_iter_rev(s, &_process_sorl_item_rev);
  if (iter_errs > 0) {
    if (!quiet) printf("ERR: sorl - Testing iter; errors reported.\n");
    return false;
  }
  return true;
}


int test_sorted_list(bool quiet)
{
  int errs = 0;
  if (!_test_sorl_create(quiet)) errs++;
  if (!_test_sorl_insert_item(quiet)) errs++;
  if (!_test_sorl_remove(quiet)) errs++;
  if (!_test_sorl_size(quiet)) errs++;
  if (!_test_sorl_first(quiet)) errs++;
  if (!_test_sorl_last(quiet)) errs++;
  if (!_test_sorl_next(quiet)) errs++;
  if (!_test_sorl_prev(quiet)) errs++;
  if (!_test_sorl_iter(quiet)) errs++;
  if (!_test_sorl_rev_iter(quiet)) errs++;

  if (!quiet) {
    if (errs) {
      printf("[FAIL] : Sorted List\n");
    } else {
      printf("[OK]   : Sorted List\n");
    }
  }
  return errs;
}
