#ifndef _OC_MEM_H
#define _OC_MEM_H
/* ------------------------------------------------------------------------- *\
   Overclocked Memory Support
     - for those occasions when you want a data structure to take ownership
       of the memory of items it contains, you can use an object destructor
       to make it happen. Note that the standard free is a correct object
       destructor, if you only need shallow memory release.
   -------------------------------------------------------------------------
   LICENSE: This program is free software. You can modify it and/or re-
   distribute it under the terms of the Apache 2.0 License. You should have
   received a copy of the Apache 2.0 License along with this program. If
   not, please see: http://www.apache.org/licenses/LICENSE-2.0.txt

   DISCLAIMER: This program is distributed in the hope that it will be
   useful, but without any warrantee; without even the implied warantee
   of fitness for any particular purpose. See the License for more details.
\* ------------------------------------------------------------------------- */

typedef void(*object_destructor)(void*);
typedef void(*map_destructor)(void*, void*);

#endif
