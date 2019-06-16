/* ------------------------------------------------------------------------- *\
   unit tests for splay tree
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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "splay-tree.h"

int test_splay_tree(bool);

/* ------------------------------------------------------------------------- *\
   Private Declarations
\* ------------------------------------------------------------------------- */

static bool _test_create(bool);
static bool _test_free(bool);
static bool _test_put_get(bool);
static bool _test_remove(bool);

static int  _compare(void*, void*);
static void _fake_free(void*, void*);
/* debugging tool..
static void _print_tree(void*, void*);
*/

/* ------------------------------------------------------------------------- *\
   Private Implementations
\* ------------------------------------------------------------------------- */

static bool _test_create( bool quiet )
{
  bool result = true;
  splay *s = splay_create(&_compare, &_fake_free);
  uint32_t sz = splay_count(s);
  if (sz != 0) {
    if (!quiet)
      printf("ERR: Creating Splay Tree failed test.\n");
    result = false;
  }
  splay_free(s);
  return result;
}

static int free_ctr = 0;
static void _fake_free(void *k, void *v)
{
  (void)k;
  (void)v;
  free_ctr++;
}

static bool _test_free( bool quiet )
{
  bool result = true;
  splay *s = splay_create(&_compare, &_fake_free);
  char *k1 = "aaa";
  char *k2 = "bbb";
  char *k3 = "ccc";
  char *vv = "val";
  splay_put(s, k1, vv);
  splay_put(s, k2, vv);
  splay_put(s, k3, vv);
  free_ctr = 0;
  splay_free(s);
  if (free_ctr != 3) {
    if (!quiet)
      printf("ERR: Splay Tree did not free items as expected.\n");
    result = false;
  }
  return result;
}

static int
_compare(void *a, void *b)
{
  return strcmp((const char*)a,(const char*)b);
}

static bool
_test_put_get(bool quiet)
{
  bool result = true;
  splay *s = splay_create(&_compare, NULL);
  char *k1 = "aaa";
  char *k2 = "bbb";
  char *k3 = "ccc";
  char *k4 = "ddd";
  char *k5 = "eee";
  char *k6 = "fff";
  char *k7 = "ggg";
  char *vv = "val";
  char *seek = "find me";
  splay_put(s, k1, vv);
  splay_put(s, k6, vv);
  splay_put(s, k5, vv);
  splay_put(s, k2, vv);
  splay_put(s, k3, seek);
  splay_put(s, k7, vv);
  splay_put(s, k4, vv);
  char* test_val = splay_get(s, "ccc");
  if (test_val != seek) {
    if (!quiet)
      printf("ERR: Splay Tree put or get items as expected.\n");
    result = false;
  }
  return result;
}

/* useful for debugging
static void
_print_tree(void *key, void *val)
{
  printf("K: %s V: %s\n", (char*)key, (char*)val);
}
*/

static bool
_test_remove(bool quiet)
{
  char *test_val;
  bool result = true;
  splay *s = splay_create(&_compare, NULL);
  char keys[30][20] = {"the", "imagination", "of", "nature", "is", "far", "greater",
    "than", "that", "mans", "own","reality","must","take","precedence","over",
    "public","relations","for","it","alone","cannot","be","fooled","modified",
    "quotes","by","richard","p.","feynman"};
  char *vv = "val";
  for(int i = 0; i < 30; i++) {
    splay_put(s,keys[i],vv);
  }
  splay_remove(s, "for");
  test_val = splay_get(s, "for");
  if (test_val != NULL) {
    if (!quiet)
      printf("ERR: Splay Tree remove did not work as expected.\n");
    result = false;
  }
  return result;
}

static bool
_test_rich_use(bool quiet)
{
  char *test_val;
  bool result = true;
  splay *s = splay_create(&_compare, NULL);
  char keys[30][20] = {"the", "imagination", "of", "nature", "is", "far", "greater",
    "than", "that", "mans", "own","reality","must","take","precedence","over",
    "public","relations","for","it","alone","cannot","be","fooled","modified",
    "quotes","by","richard","p.","feynman"};
  char *vv = "val";
  for(int i = 0; i < 30; i++) {
    splay_put(s,keys[i],vv);
  }
  splay_get(s,keys[7]);
  splay_get(s,keys[9]);
  splay_get(s,keys[13]);
  splay_get(s,keys[19]);
  splay_get(s,keys[21]);
  splay_get(s,keys[5]);
  splay_get(s,keys[7]);
  splay_get(s,keys[4]);
  splay_get(s,keys[7]);
  splay_get(s,keys[7]);
  splay_get(s,keys[7]);
  splay_remove(s,keys[7]);
  splay_get(s,keys[9]);
  splay_get(s,keys[13]);
  splay_get(s,keys[19]);
  splay_get(s,keys[21]);
  splay_get(s,keys[5]);
  splay_get(s,keys[9]);
  splay_get(s,keys[13]);
  splay_get(s,keys[19]);
  splay_get(s,keys[21]);
  splay_get(s,keys[5]);
  splay_get(s,keys[9]);
  splay_get(s,keys[13]);
  splay_get(s,keys[19]);
  splay_get(s,keys[21]);
  splay_get(s,keys[5]);
  splay_put(s,keys[7],"Hello world");
  splay_get(s,keys[21]);
  splay_get(s,keys[5]);
  splay_get(s,keys[4]);
  splay_get(s,keys[9]);
  splay_get(s,keys[12]);
  splay_get(s,keys[17]);
  splay_get(s,keys[1]);
  splay_get(s,keys[0]);
  splay_get(s,keys[1]);
  test_val = splay_get(s,keys[7]);
  if (strcmp(test_val,"Hello world") != 0) {
    if (!quiet)
      printf("ERR: Splay Tree failed in mixed use. (replaced key)\n");
      result = false;
  }
  splay_remove(s,keys[29]);
  splay_get(s,keys[5]);
  splay_get(s,keys[4]);
  splay_get(s,keys[9]);
  splay_get(s,keys[12]);
  splay_get(s,keys[17]);
  splay_get(s,keys[1]);
  splay_get(s,keys[0]);
  splay_get(s,keys[1]);
  if (splay_count(s) != 29) {
    if (!quiet)
      printf("ERR: Splay Tree failed in mixed use. (count off 1)\n");
      result = false;
  }
  splay_get(s,keys[5]);
  splay_get(s,keys[4]);
  splay_get(s,keys[9]);
  splay_get(s,keys[12]);
  splay_get(s,keys[17]);
  splay_get(s,keys[1]);
  splay_get(s,keys[0]);
  splay_get(s,keys[1]);
  splay_put(s,"be","nother test"); /* should replace, not add. */
  if (splay_count(s) != 29) {
    if (!quiet)
      printf("ERR: Splay Tree failed in mixed use. (count off 2)\n");
      result = false;
  }
  splay_get(s,keys[5]);
  splay_get(s,keys[4]);
  splay_get(s,keys[9]);
  splay_get(s,keys[12]);
  splay_get(s,keys[17]);
  splay_get(s,keys[1]);
  splay_get(s,keys[0]);
  splay_get(s,keys[1]);
  test_val = splay_get(s,"be");
  if (strcmp(test_val,"nother test") != 0) {
    if (!quiet)
      printf("ERR: Splay Tree failed in mixed use. (replaced key 2)\n");
      result = false;
  }
  return result;

}

/* ------------------------------------------------------------------------- *\
   Public Interface
\* ------------------------------------------------------------------------- */

int test_splay_tree( bool quiet )
{
  uint32_t errs = 0;

  if (_test_create(quiet) != true) errs++;
  if (_test_free(quiet) != true) errs++;
  if (_test_put_get(quiet) != true) errs++;
  if (_test_remove(quiet) != true) errs++;
  if (_test_rich_use(quiet) != true) errs++;

  if (!quiet) {
    if (errs)
      printf("[FAIL] : Splay Tree\n");
    else
      printf("[OK]   : Splay Tree\n");
  }

  return errs;
}
