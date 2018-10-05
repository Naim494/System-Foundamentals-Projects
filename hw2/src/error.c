/*
 * Error handling routines
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

int errors;
int warnings;
int dbflag = 1;

// void fatal(fmt, a1, a2, a3, a4, a5, a6)
// char *fmt, *a1, *a2, *a3, *a4, *a5, *a6;
// {
//         fprintf(stderr, "\nFatal error: ");
//         fprintf(stderr, fmt, a1, a2, a3, a4, a5, a6);
//         fprintf(stderr, "\n");
//         exit(1);
// }

void fatal(char *fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        fprintf(stderr, "\nFatal error: ");
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");
        va_end(ap);
        exit(1);
}


// void error(fmt, a1, a2, a3, a4, a5, a6)
// char *fmt, *a1, *a2, *a3, *a4, *a5, *a6;
// {
//         fprintf(stderr, "\nError: ");
//         fprintf(stderr, fmt, a1, a2, a3, a4, a5, a6);
//         fprintf(stderr, "\n");
//         errors++;
// }

void error(char *fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        fprintf(stderr, "\nError: ");
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");
        va_end(ap);
        errors++;
}

// void warning(fmt, a1, a2, a3, a4, a5, a6)
// char *fmt, *a1, *a2, *a3, *a4, *a5, *a6;
// {
//         fprintf(stderr, "\nWarning: ");
//         fprintf(stderr, fmt, a1, a2, a3, a4, a5, a6);
//         fprintf(stderr, "\n");
//         warnings++;
// }

void warning(char *fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        fprintf(stderr, "\nWarning: ");
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");
        warnings++;
}

// void debug(fmt, a1, a2, a3, a4, a5, a6)
// char *fmt, *a1, *a2, *a3, *a4, *a5, *a6;
// {
//         if(!dbflag) return;
//         fprintf(stderr, "\nDebug: ");
//         fprintf(stderr, fmt, a1, a2, a3, a4, a5, a6);
//         fprintf(stderr, "\n");
// }

void debug(char *fmt, ...) {
        if(!dbflag) return;
        va_list ap;
        va_start(ap, fmt);
        fprintf(stderr, "\nDebug: ");
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");
}
