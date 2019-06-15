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
#include <stdio.h>

int test_cmd_line_yn( bool );

int test_cmd_line_yn(bool quiet)
{
  if (!quiet) {
    printf("[OK]   : Command Line Y or N.\n");
  }
  return 0;
}
