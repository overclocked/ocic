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
#include <stdio.h>

/* ------------------------------------------------------------------------- *\
   data structures
\* ------------------------------------------------------------------------- */

typedef struct splay_node {
  void   *k;
  void   *v;
  struct  splay_node *l;
  struct  splay_node *r;
} splay_node;

typedef struct splay_seq {
  splay_node *ggp, *gp, *p;
} splay_seq;

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
static void* _find(splay *s, splay_seq *seq, splay_node *sn, void *seek);
static void _splay(splay *s, splay_seq *seq, splay_node *sn);
static inline void _rotate_left(splay_node *gp, splay_node *p, splay_node *c);
static inline void _rotate_right(splay_node *gp, splay_node *p, splay_node *c);
static void _remove(splay *s, splay_node *p, splay_node *sn);
static void _traverse(splay_node *, void(*)(void*,void*));
static inline void _shift_seq(splay_seq *seq, splay_node *sn);
static inline void _set_gp(splay_node *gp, splay_node *p, splay_node *sn);
static void _print_tree(splay_node *sn, int depth, int dir);

/* ------------------------------------------------------------------------- *\
   testing support declarations
\* ------------------------------------------------------------------------- */

void splay_print_tree(splay *s);

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
      s->count++;
    }
  } else {
    if (curr->r) {
      _insert_node(s, curr->r, ins);
    } else {
      curr->r = ins;
      s->count++;
    }
  }
  return;
}

static inline void
_shift_seq(splay_seq *seq, splay_node *sn)
{
  seq->ggp = seq->gp;
  seq->gp = seq->p;
  seq->p = sn;
}

/* Could be recursive; but let's not be clever. */
static void*
_find(splay *s, splay_seq *seq, splay_node *sn, void *seek)
{
  int dir;
  while(sn) {
    dir = s->cmp(seek, sn->k);
    if (dir == 0) {
      _splay(s, seq, sn);
      return sn->v;
    }
    _shift_seq(seq, sn);
    if (dir < 0) {
      sn = sn->l;
    } else {
      sn = sn->r;
    }
  }
  /* Not found. */
  return NULL;
}

static inline void
_set_gp(splay_node *gp, splay_node *p, splay_node *sn)
{
  if (gp->l == p) gp->l = sn;
  else            gp->r = sn;
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

 Note that the change is in effect regardless of whether P is l or r on GP,
 but we need to correct the correct direction on GP.

*/
static inline void _rotate_right(splay_node *gp, splay_node *p, splay_node *c)
{
    splay_node *v3 = c->r; /* Could be null */
    c->r = p;
    p->l = v3;
    if (gp) _set_gp(gp, p, c);
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

*/
static inline void _rotate_left(splay_node *gp, splay_node *p, splay_node *c)
{
  splay_node *v2 = c->l; /* Could be null */
  c->l = p;
  p->r = v2; /* was c */
  if (gp) _set_gp(gp, p, c);
}

static void _splay(splay *s, splay_seq *seq, splay_node *sn)
{
  /* No op if root. */
  if (!seq->p) return;

  /* Are we going to set to root? */
  if (!seq->ggp || !seq->gp) {
    s->root = sn;
  }

  /* Determine splay action */
  if(!seq->gp) {
    /* Parent is root: simple case. */
    if (seq->p->l == sn) {
      _rotate_right(NULL, seq->p, sn);
    } else {
      _rotate_left(NULL, seq->p, sn);
    }
    return;
  }

  /* The two-step : rotation sequence varies */
  /* PROBLEM!!! the top down sequences don't work because the hierarchy has
     changed. */
  if (seq->p->l == sn && seq->gp->l == seq->p) {
    _rotate_right(seq->ggp, seq->gp, seq->p);
    _rotate_right(seq->ggp, seq->p, sn);
  } else if (seq->p->r == sn && seq->gp->r == seq->p) {
    _rotate_left(seq->ggp, seq->gp, seq->p);
    _rotate_left(seq->ggp, seq->p, sn);
  } else if (seq->p->l == sn) {
    _rotate_right(seq->gp, seq->p, sn);
    _rotate_left(seq->ggp, seq->gp, sn);
  } else {
    _rotate_left(seq->gp, seq->p, sn);
    _rotate_right(seq->ggp, seq->gp, sn);
  }

}

void
_remove(splay *s, splay_node *p, splay_node *sn)
{
  splay_node *l, *r, *orphan;
  if (!sn) return;
  l = sn->l;
  r = sn->r;
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
    } else if (r) {
      p->l = r;
    }
  }

  /* Update children */
  if (l) {
    orphan = l->r;
    l->r = r;
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

void
_print_tree(splay_node *sn, int depth, int dir)
{
  if (!sn) return;
  for (int i = 0; i < depth; i++) {
    putchar('|');
  }
  if (dir == 0) {
    putchar('*');
  } else if (dir < 0) {
    putchar('<');
  } else {
    putchar('>');
  }
  printf(" %s\n",(char*)sn->k);
  depth++;
  _print_tree(sn->l, depth, -1);
  _print_tree(sn->r, depth, 1);
}

void
splay_iter(splay *s, void(*handle)(void*, void*))
{
  _traverse(s->root, handle);
}

void
splay_print_tree(splay *s) {
  _print_tree(s->root, 0, 0);
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
    splay_seq seq = {0};
    return _find(s, &seq, s->root, key);
  }
}

void
splay_remove(splay *s, void *key)
{
  int dir;
  splay_node *sn = s->root;
  splay_node *p = NULL;
  while(sn) {
    dir = s->cmp(key, sn->k);
    if (!dir) {
      _remove(s, p, sn);
      s->count--;
      return;
    }
    p = sn;
    if (dir < 0) {
      sn = sn->l;
    } else {
      sn = sn->r;
    }
  }
}

uint32_t
splay_count(splay *s)
{
  return s->count;
}
