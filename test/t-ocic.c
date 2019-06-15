#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "t-ocic.h"

int main(int c, char ** argv) {

	bool quiet = (c > 1 && !strcmp(argv[1], "-q"));

	int errs = 0;

  errs += test_cmd_line_yn(quiet);
	errs += test_singly_linked_list(quiet);
	errs += test_sorted_list(quiet);

	if (!quiet && errs > 0) {
		printf("Total Unit Test Errors Reported: %d", errs);
	}

	return errs;
}
