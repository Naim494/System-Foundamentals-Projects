#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>

#include "imprimer.h"

/*
 * "Imprimer" printer spooler.
 */

int main(int argc, char *argv[])
{
    char optval;
    while(optind < argc) {
	if((optval = getopt(argc, argv, "")) != -1) {
	    switch(optval) {
	    case '?':
		fprintf(stderr, "Usage: %s [-i <cmd_file>] [-o <out_file>]\n", argv[0]);
		exit(EXIT_FAILURE);
		break;
	    default:
		break;
	    }
	}
    }
    exit(EXIT_SUCCESS);
}
