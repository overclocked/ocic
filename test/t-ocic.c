#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main(int c, char ** argv) {

	bool quiet = (c > 1 && !strcmp(argv[1], "-q"));	

	if (!quiet) {
		printf("[OK] cmd_line_yn\n"); /* No op. No Test needed. */
	}

	return 0;
}
