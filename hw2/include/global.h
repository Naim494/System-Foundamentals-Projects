
/*
 * Global definitions
 */

#define TRUE 1
#define FALSE 0

#define EPSILON 1e-6            /* Don't divide by anything smaller */

typedef struct Ifile {
        FILE *fd;
        char *name;
        int line;
        struct Ifile *prev;
} Ifile;

