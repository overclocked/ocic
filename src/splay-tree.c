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

#include <stdlib.h>
#include "splay-tree.h"

/* ------------------------------------------------------------------------- *\
   data structures
\* ------------------------------------------------------------------------- */

typedef struct splay_node {
  void   *k;
  void   *v;
  struct  splay_node *l;
  struct  splay_node *r;
  struct  splay_node *p;
} splay_node;

struct splay {
  comparator      cmp;
  map_destructor  rel;
  splay_node     *root;
  uint32_t        count;
};

/* ------------------------------------------------------------------------- *\
   private method declarations
\* ------------------------------------------------------------------------- */

static void _free_tree(splay *s, splay_node *sn);
static void _insert_node(splay *s, splay_node *curr, splay_node *ins);
static void* _find(splay *s, splay_node *sn, void *seek);
static void _splay(splay *s, splay_node *sn);
static inline void _rotate_left(splay_node *p, splay_node *c);
static inline void _rotate_right(splay_node *p, splay_node *c);
static void _remove(splay *s, splay_node *sn);
static void _traverse(splay_node *, void(*)(void*,void*));

/* ------------------------------------------------------------------------- *\
   testing support declarations
\* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *\
   private method implementations
\* ------------------------------------------------------------------------- */

static void
_free_tree(splay *s, splay_node *sn) {
  if (sn->l) _free_tree(s, sn->l);
  if (sn->r) _free_tree(s, sn->r);
  if (s->rel) {
    s->rel(sn->k, sn->v);
  }
  s->count--;
  free(sn);
}

static void
_insert_node(splay *s, splay_node *curr, splay_node *ins)
{
  int dir = s->cmp(ins->k, curr->k);

  if (dir == 0) {
    /* Matching key: replace node. */
    if (s->rel) {
      s->rel(curr->k, curr->v);
    }
    curr->k = ins->k;
    curr->v = ins->v;
    free(ins);
  } else if (dir < 0) {
    if (curr->l) {
      _insert_node(s, curr->l, ins);
    } else {
      curr->l = ins;
      ins->p = curr;
      s->count++;
    }
  } else {
    if (curr->r) {
      _insert_node(s, curr->r, ins);
    } else {
      curr->r = ins;
      ins->p = curr;
      s->count++;
    }
  }
  return;
}

/* Could be recursive; but let's not be clever. */
static void*
_find(splay *s, splay_node *sn, void *seek)
{
  int dir;
  while(sn) {
    dir = s->cmp(seek, sn->k);
    if (dir == 0) {
      _splay(s, sn);
      return sn->v;
    }
    if (dir < 0) {
      sn = sn->l;
    } else {
      sn = sn->r;
    }
  }
  /* Not found. */
  return NULL;
}

/*  Rotate Right (C moves up and to the right; P moves down and to the right)

        GP                       GP
       /  \                     /  \
      P    *                   C    *
     / \                      / \
    C   V1        ====>     V2   P
   / \                          /  \
  V2  V3                      V3    V1

 Changes: (downward)
   V3 Moves from C->R to P->L
   GP->L moves from P to C
   C->R moves from V3 to P
           (upward)
   P->P is now C
   V3->P is now P
   C->P is now GP

 Note that the change is in effect regardless of whether P is l or r on GP,
 but we need to correct the correct direction on GP.

*/
static inline void _rotate_right(splay_node *p, splay_node *c)
{
    splay_node *gp = p->p; /* Could be null */
    splay_node *v3 = c->r; /* Could be null */
    c->r = p;
    c->p = gp;
    p->l = v3;
    p->p = c;
    if (v3) v3->p = p;
    /* reposition in parental structure (unless there is no grandparent) */
    if (gp) {
      if (gp->l == p) {
        gp->l = c;
      } else {
        gp->r = c;
      }
    }
}

/*  Rotate Left (C moves up and to the left; P moves down and to the left)

        GP                       GP
       /  \                     /  \
      P    *                   C    *
     / \                      / \
   V1   C        ====>       P   V3
       / \                  / \
     V2   V3              V1   V2

 Changes: (downward)
   V2 Moves from C->L to P->R
   GP->L moves from P to C (or GP->R; either way, correct accordingly.)
   C->L moves from V2 to P
           (upward)
   P->P is now C
   V2->P is now P
   C->P is now GP

*/
static inline void _rotate_left(splay_node *p, splay_node *c)
{
  splay_node *gp = p->p; /* Could be null */
  splay_node *v2 = c->l; /* Could be null */
  c->l = p;
  c->p = gp;
  p->r = v2; /* was c */
  p->p = c;
  if (v2) v2->p = p;
  /* reposition in parental structure (unless there is no grandparent) */
  if (gp) {
    if (gp->l == p) {
      gp->l = c;
    } else {
      gp->r = c;
    }
  }
}

static void _splay(splay *s, splay_node *sn)
{
  splay_node *p, *gp;
  /* No op if root. */
  if (!sn->p) return;

  p = sn->p;
  gp = p->p;

  /* Determine splay action */
  if(!gp) {
    /* Parent is root: simple case. */
    if (p->l == sn) {
      _rotate_right(p, sn);
    } else {
      _rotate_left(p, sn);
    }
    s->root = sn;
  } else {
    /* We have a grandparent; which also could be root. */
    if (!gp->p) {
      s->root = sn;
    }
    /* Determine rotation sequence */
    if (p->l == sn && gp->l == p) {
      _rotate_right(gp, p);
      _rotate_right(p, sn);
    } else if (p->r == sn && gp->r == p) {
      _rotate_left(gp, p);
      _rotate_left(p, sn);
    } else if (p->l == sn) {
      _rotate_right(p, sn);
      _rotate_left(gp, sn);
    } else {
      _rotate_left(p, sn);
      _rotate_right(gp, sn);
    }
  }
}

void
_remove(splay *s, splay_node *sn)
{
  splay_node *l, *r, *p, *orphan;
    char *lk, *rk, *pk;
  if (!sn) return;
  l = sn->l;
  r = sn->r;
  p = sn->p;
    if (l) lk = l->k; else lk = "NA";
    if (r) rk = r->k; else rk = "NA";
    if (p) pk = p->k; else pk = "NA";
  orphan = NULL;
  if (!p) {
    /* Need to update root */
    if (l) s->root = l;
    else   s->root = r;
  }
  /* Actual tree repair */

  /* Update parental bonds, if needed. */
  if (p) {
    if (l) {
      p->l = l;
      l->p = p;
    } else if (r) {
      p->l = r;
      r->p = p;
    }
  } else {
    if (l) {
      l->p = NULL;
    } else {
      r->p = NULL;
    }
  }

  /* Update children */
  if (l) {
    orphan = l->r;
    l->r = r;
    if (r) r->p = l;
  }

  /* Find a home for an orphan */
  if (orphan) {
    while (l) {
      if (s->cmp(orphan->k, l->k) < 0) {
        if (l->l) l = l->l;
        else {
          l->l = orphan;
          break;
        }
      } else {
        if (l->r) l = l->r;
        else {
          l->r = orphan;
          break;
        }
      }
    }
  }

  /* free the node */
  if (s->rel) {
    s->rel(sn->k, sn->v);
  }
  free(sn);
}

void
_traverse(splay_node *sn, void(*handle)(void*, void*))
{
  if (sn->l) {
    _traverse(sn->l, handle);
  }
  handle(sn->k, sn->v);
  if (sn->r) {
    _traverse(sn->r, handle);
  }
}

/* ------------------------------------------------------------------------- *\
   testing support implementations
\* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- *\
   public methods
\* ------------------------------------------------------------------------- */

splay*
splay_create(comparator compare, map_destructor release)
{
  splay *s;
  if (!compare) return NULL;
  s       = malloc(sizeof(splay));
  s->cmp  = compare;
  s->rel  = release;
  s->root = NULL;
  return s;
}

void
splay_free(splay *s)
{
  if (s->root) {
    _free_tree(s, s->root);
  }
  free(s);
  return;
}

void
splay_put(splay *s, void *key, void *val)
{
    splay_node *sn = malloc(sizeof(splay_node));
    sn->l = NULL;
    sn->r = NULL;
    sn->p = NULL;
    sn->k = key;
    sn->v = val;
    if (!s->root) {
      s->root = sn;
      s->count++;
    } else {
      _insert_node(s, s->root, sn);
    }
}

void*
splay_get(splay *s, void *key)
{
  if (!s->root) return NULL;
  if (s->cmp(s->root->k, key) == 0) {
    return s->root->v;
  } else {
    return _find(s, s->root, key);
  }
}

void
splay_remove(splay *s, void *key)
{
  int dir;
  splay_node *sn = s->root;
  while(sn) {
    dir = s->cmp(key, sn->k);
    if (!dir) {
      _remove(s, sn);
      s->count--;
      return;
    }
    if (dir < 0) {
      sn = sn->l;
    } else {
      sn = sn->r;
    }
  }
}

void
splay_iter(splay *s, void(*handle)(void*, void*))
{
  _traverse(s->root, handle);
}

uint32_t
splay_count(splay *s)
{
  return s->count;
}
