#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[]) {
    sf_mem_init();

    // double* ptr = sf_malloc(sizeof(double));

    // sf_show_heap();

    // double* ptr1 = sf_malloc(53);

    // sf_show_heap();

    // double* ptr2 = sf_malloc(4060);

    // void *x = sf_malloc(48);
    // sf_show_heap();
    // int *z = sf_malloc(sizeof(int));
    // sf_show_heap();
    // void *y = sf_malloc(48);

    // *z = 4;

    // sf_show_heap();

    // sf_free(x);

    // sf_show_heap();

    // sf_free(y);

    // sf_show_heap();

    // sf_free(z);

    // int *x = NULL;

    // int *y = sf_realloc(x, sizeof(double));

    // if(y != NULL)
    //     *y = 9;

     int *x = sf_malloc(sizeof(double));

     sf_show_heap();

     *x = 10;

    sf_free(x);



    sf_show_heap();


    // double* ptr = sf_malloc(sizeof(double));

    // *ptr = 8;

    //*ptr = 320320320e-320;

    // printf("%f\n", *ptr);

    // sf_free(ptr);

    sf_mem_fini();

    return EXIT_SUCCESS;
}
