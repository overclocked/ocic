#ifndef _COMPARATOR_H
#define _COMPARATOR_H
/* ------------------------------------------------------------------------- *\
   Comparator
     - the comparator function should return < 0 if the first item should
       precede the second;
     - 0 if the items should be considered equal
     - > 0 if the first item should follow the second.
   -------------------------------------------------------------------------
   LICENSE: This program is free software. You can modify it and/or re-
   distribute it under the terms of the Apache 2.0 License. You should have
   received a copy of the Apache 2.0 License along with this program. If
   not, please see: http://www.apache.org/licenses/LICENSE-2.0.txt

   DISCLAIMER: This program is distributed in the hope that it will be
   useful, but without any warrantee; without even the implied warantee
   of fitness for any particular purpose. See the License for more details.
\* ------------------------------------------------------------------------- */

typedef int(*comparator)(void*,void*);

#endif
