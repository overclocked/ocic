/* ------------------------------------------------------------------------- *\
   unit tests for hash map
   includes some optional collision metrics
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
#include <stdio.h>

#include "hash-map.h"

/* declarations of hash-map internals */
uint32_t _hmap_size(hmap*);
uint32_t _hmap_collisions(hmap*);

/* public entry point */
int test_hash_map(bool);

/* helper declarations */
static void _fake_free(void*, void*);

/* test declarations */
static bool _test_create(bool);
static bool _test_free(bool);
static bool _test_put_get(bool);
static bool _test_collisions(bool);
static bool _test_remove(bool);

/* helper functions */
static int _free_ctr = 0;
static void _fake_free(void* key, void* obj)
{
  (void)key;
  (void)obj;
  _free_ctr++;
}

/* test implementations */
static bool _test_create(bool quiet)
{
  bool result = true;
  hmap *h = hmap_create(NULL);
  uint32_t c = hmap_count(h);
  uint32_t z = _hmap_size(h);
  if (c != 0 || z != 1024) {
    if (!quiet) printf("ERR: Hash Map improperly created.\n");
    result = false;
  }
  hmap_free(h);
  return result;
}

static bool _test_free(bool quiet)
{
  bool result = true;
  hmap *h = hmap_create(&_fake_free);
  char *k1 = "keya";
  char *i1 = "vala";
  char *k2 = "keyb";
  char *i2 = "valb";
  hmap_put(h, k1, i1);
  hmap_put(h, k2, i2);
  _free_ctr = 0;
  hmap_free(h);
  if (_free_ctr != 2) {
    if (!quiet) printf("ERR: Hash Map not calling destructor as expected.\n");
    result = false;
  }
  return result;
}

static bool _test_put_get(bool quiet)
{
  bool result = true;
  hmap *h = hmap_create(NULL);
  char *k1 = "keya";
  char *i1 = "vala";
  char *k2 = "keyb";
  char *i2 = "valb";
  hmap_put(h, k1, i1);
  hmap_put(h, k2, i2);
  char *r2 = hmap_get(h, "keyb");
  if (r2 != i2) {
    if (!quiet) printf("ERR: Hash Map err testing item put: %s\n", r2);
    result = false;
  }
  return result;
}

static bool _test_collisions(bool quiet)
{
  bool result = true;
  int  coll;
  hmap *h = hmap_create(NULL);
  char *colval1 = "collision";
  char *colval2 = "collider";
  char *v1 = "val1";
  char *v2 = "val2";
  char key[1000][10];
  char *lkey[100] = {
    "The comprehensive nature of the subject of this work renders it",
    "impossible to deal with its various divisions and sub-divisions, except",
    "in a very condensed manner, within the limits of a handbook for",
    "students.",
    "I have endeavoured to present to the reader, and to the student of",
    "ornamental and decorative art, some of the salient features which",
    "characterize the historic styles of ornament, and those that seem to me",
    "to show themselves as landmarks in the wide domain of Historic Ornament.",
    "Realistic decoration was the earliest form of all art, as we find it in",
    "the etchings on the bones drawn by the prehistoric cave-dwellers; but",
    "ornamental design or pattern drawing is a kind of invention which",
    "implies the orderly decoration of architectural forms and other objects,",
    "and is generally applied to such objects with the view of adding some",
    "enrichment that shall make them more pleasing to the sight.",
    "The former belongs more to pictorial art, while the latter is purely",
    "decorative.",
    "As the construction of ornament, in a great measure, ought to be based",
    "on the laws that govern the design of good architecture—this we gather",
    "from the design of the best ornament of the historic styles—it has been",
    "thought necessary to give a slight sketch of each of the principal",
    "orders and styles of architecture, placing them, as far as possible, in",
    "a chronological sequence in regard to the periods of their existence,",
    "and countries in which they flourished.",
    "In some cases I have also thought it desirable to give a brief account",
    "of the religion of those nations that have created distinct styles of",
    "architecture and ornament; for in many cases, such as in the art of the",
    "ancient world and of the Middle Ages, we find that the art of a country",
    "was so bound up with the religion of its people, that to understand the",
    "former it is indispensable to have some knowledge of their religious",
    "ceremonies and beliefs.",
    "I have here to express my indebtedness to various writers on ornamental",
    "art whom I have named in the pages of these volumes for some useful",
    "points of information, and to them and the publishers of this work for",
    "the use of the greater portion of the blocks of illustrations.",
    "I have also to thank Mr. T. M. Lindsay for the use of his drawing of the",
    "monument of Lysikrates, and the Science and Art Department for",
    "permission to use many of the illustrations of their excellent handbooks",
    "on decorative art.",
    "In a succeeding volume to this work, the various divisions of the",
    "Industrial Arts and Crafts will be treated in their historical",
    "developments of decoration and workmanship.",
    "In conclusion, I trust that the contents of these pages will be helpful",
    "to students in art schools, and to others who may desire to have an",
    "introduction to the fascinating study of Historic Ornament.",
    "As already remarked, we are hampered by tradition in our attempts to",
    "produce originality in ornament, but there is very little tradition for",
    "the absolute copying of a particular style, except from nations who have",
    "had no decided art of their own. As far as we know of the history and",
    "practice in the whole field of ornamental design, from its remote",
    "beginnings it has been mostly all along a series of systems of",
    "developments, sometimes for good and sometimes for the opposite, but",
    "rarely, if ever, a system of copying. Some notable exceptions to this",
    "may be noticed, as when, for the expediencies known as “tricks of the",
    "trade,” the Phœnicians made ivory carvings in exact imitation of",
    "Egyptian designs, and sold them to the Assyrians; and likewise bronze",
    "bowls and platters in both Assyrian and Egyptian imitations, and traded",
    "with them throughout the Ægean and Mediterranean, or when the",
    "Siculo-Arabian silks were made at Palermo in imitation of Saracen",
    "designs, with mock-Saracenic inscriptions, and sold for the real",
    "articles. Other instances might be cited, but these were among the most",
    "successful.",
    "As regards the purity of styles it may be safely said, that, with rare",
    "exceptions, it is well-nigh impossible to find a well-designed and",
    "complete scheme of decoration, or a building that will stand the test of",
    "having perfect unity in style; in fact, it may be more artistic on",
    "account of its incompleteness in this respect, for any work of art that",
    "is designed by receipt, like the Egyptian temples or Mohammadan",
    "ornament, is rather wearisome. It is pleasant to see at times a little",
    "bit showing here and there of the designer’s individuality. When the",
    "monotonous repetition of the laws peculiar to any arbitrary style are",
    "broken by a wilful and, perhaps, sinful artist, we often get a",
    "refreshing and original rendering that is not by any means displeasing.",
    "In transitional design from one style to another, much beautiful work",
    "may be seen. In connection with this the Byzantine style may be",
    "mentioned, with its Classic and Oriental forms, Elizabethan, Jacobean,",
    "Lombard Gothic, and the French styles of Henri Deux and François 1^{er},",
    "in most of which Gothic and Renaissance forms are happily blended; and",
    "in the beautiful Siculo-Arabian textiles, where Italian and Saracenic",
    "forms make an interesting union. We learn from these examples that the",
    "successful designer of ornament should have a thorough knowledge of the",
    "historic styles, not for the purpose of reproducing their forms, but in",
    "order to discover for himself the methods by which the old artists",
    "arrived at the successful treatment of nature and of former styles, so",
    "that by the application of his knowledge, derived from the study of",
    "nature and the works of former artists, he may be enabled to give to the",
    "world some original and interesting work.    ",
    "The first indications of the presence of man in Britain was brought to",
    "light in the shape of a flint flake found by the Rev. O. Fisher, in the",
    "presence of Professor W. Boyd Dawkins, in the lower brick earth of the",
    "Stoneham pit at Crayford, in Kent, in the year 1872. In the year 1876 a",
    "second flake was found in a similar situation at Erith, in Kent,",
    "considerably worn by use. This form of implement was used in the late",
    "Pleistocene age, and also in the Neolithic (Newer Stone age) and Bronze",
    "ages. It was employed in the historic ages by the Egyptians, and by the",
    "Romanized Britons of Sussex, in whose tombs it has been found. This",
    "implement is the latest survival of the Palæolithic age. Geologists have",
    "proved that Ireland, England and Europe were united in the Palæolithic",
    "age, and this accounts for the similarity of stone implements and other",
    "remains found in the river-drift deposits, in caves, and other",
    "situations in the river valley over this vast area. The roughly chipped"
  };
  for (int i = 0; i < 333; i++) {
    snprintf(key[i], 10, "nkey%d", i);
  }
  for (int i = 333; i < 667; i++) {
    snprintf(key[i], 10, "%dkey", i);
  }
  for (int i = 667; i < 1000; i++) {
    snprintf(key[i], 10, "abc%defg", i);
  }
  for (int i = 0; i < 1000; i++) {
    if (i == 9) {
      hmap_put(h, key[i], colval1);
    } else if (i == 80) {
      hmap_put(h, key[i], colval2);
    } else {
      hmap_put(h, key[i], v1 );
    }
  }

  for (int i = 0; i < 100; i++) {
    hmap_put(h, lkey[i], v2);
  }

  coll = _hmap_collisions(h);
  if (coll > 400) {
    if (!quiet) {
      printf("WARN: Are you collision tuning? You are at: %d\n", coll);
    }
  }

  if (hmap_get(h, "nkey9") != colval1 || hmap_get(h, "nkey80") != colval2) {
    if (!quiet) {
      printf("ERR: Collision resolution failed.\n");
      result = false;
    }
  }
  hmap_free(h);
  return result;
}

bool _test_remove(bool quiet)
{
  bool result = true;
  hmap *h = hmap_create(NULL);
  char *k = "key";
  char *v = "val";
  hmap_put(h, k, v);
  if (hmap_get(h, k) != v) {
    if (!quiet) {
      printf("ERR: Unexpected error retrieving value, while testing remove.\n");
      result = false;
    }
  }
  hmap_remove(h, k);
  if (hmap_get(h, k) != NULL) {
    if (!quiet) {
      printf("ERR: Remove fail.\n");
      result = false;
    }
  }
  if (hmap_count(h) > 0) {
    if (!quiet) {
      printf("ERR: Remove count not adjusted.\n");
      result = false;
    }
  }
  hmap_free(h);
  return result;
}

int test_hash_map(bool quiet)
{
  uint32_t errs = 0;

  if (_test_create(quiet) != true) errs++;
  if (_test_free(quiet) != true) errs++;
  if (_test_put_get(quiet) != true) errs++;
  if (_test_collisions(quiet) != true) errs++;
  if (_test_remove(quiet) != true) errs++;

  if (!quiet) {
    if (errs)
      printf("[FAIL] : Hash Map\n");
    else
      printf("[OK]   : Hash Map\n");
  }

  return errs;
}
