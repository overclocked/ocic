#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "t-ocic.h"

int main(int c, char ** argv) {

	bool quiet = (c > 1 && !strcmp(argv[1], "-q"));

	int errs = 0;

  errs += test_cmd_line_yn(quiet);
	errs += test_hash_map(quiet);
	errs += test_singly_linked_list(quiet);
	errs += test_sorted_list(quiet);
	errs += test_splay_tree(quiet);

	if (!quiet) {
		if (errs)
			printf("Total Unit Test Errors Reported: %d\n", errs);
		else
			printf("Testing completed with NO errors.\n");
	}

	return errs;
}
