/* ------------------------------------------------------------------------- *\
   Command Line YN 
     - A dirt simple utility for grabbing a Yes/No response from the command
       line, which only considers Y or y to be Yes, and any other response
       to be No.
   -------------------------------------------------------------------------
   LICENSE: This program is free software. You can modify it and/or re-
   distribute it under the terms of the Apache 2.0 License. You should have
   received a copy of the Apache 2.0 License along with this program. If
   not, please see: http://www.apache.org/licenses/LICENSE-2.0.txt

   DISCLAIMER: This program is distributed in the hope that it will be
   useful, but without any warrantee; without even the implied warantee
   of fitness for any particular purpose. See the License for more details.
\* ------------------------------------------------------------------------- */  

#include "cmd_line_yn.h"

#include <stdlib.h>
#include <stdio.h>

bool yorn(const char* prompt) {
	bool answer = false;
	if (prompt) {
		printf("%s", prompt);
	}
	int c = getchar();
	if (c == 'Y' || c == 'y') answer = true;
	while (c != EOF && c != '\n') c = getchar();
	return answer;
}
