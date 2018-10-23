/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "sfmm.h"

void initialSetup();
void initialize_hd_ft(void *item, int al, int prev_al, int zrs, size_t sz, int rq_sz, int flag);

void *place_block();
sf_free_list_node *search_freelist(size_t blk_sz);
void *spit_block();
size_t total_block_sz(size_t rq_sz);
void *coalesce();

int flag = 1;

void *sf_malloc(size_t size) {

    //do initial setup if first call to sf_malloc
    if(flag){
        initialSetup();
        flag = 0;
    }

    sf_show_heap();
    // //return if size is 0
    // if(size == 0)
    //     return NULL;

    // //compute total block size (minimum is 32 bytes)
    // size_t blk_sz = total_block_sz(size);

    // //search for free block in appropriate free list node
    // sf_free_list_node *block_location = search_freelist(blk_sz);

    // if(block_location != NULL) {



    // }
    // else {

    // }




    return NULL;
}

sf_free_list_node *search_freelist(size_t blk_sz) {

    sf_free_list_node *next = sf_free_list_head.next;

    while((next != &sf_free_list_head)) {

        if(next -> size == blk_sz)
            return next;

        next = next -> next;
    }


    return NULL;
}

size_t total_block_sz(size_t rq_sz) {

    size_t total = rq_sz + 8;

    while((total % 16 != 0) && (total < 32))
        total++;

    return total;
}

void initialSetup() {

    sf_mem_grow();

    //set up prologue
    sf_prologue *prlg = (sf_prologue*)sf_mem_start();
    prlg -> padding = 0;
    prlg -> header.payload = 0;
    prlg -> header.info.allocated = 1;
    prlg -> header.info.prev_allocated = 0;
    prlg -> footer.info.allocated = 1;
    prlg -> footer.info.prev_allocated = 0;

    //set up epilogue
    sf_epilogue *eplg = (sf_epilogue*)(sf_mem_end() - sizeof(sf_epilogue));
    eplg -> footer.info.allocated = 1;
    eplg -> footer.info.prev_allocated = 1;

    //size of initial block
    size_t size = PAGE_SZ - (sizeof(sf_prologue) + sizeof(sf_epilogue));

    //create first sf_free_list_node after sentinel (created by sf_mem_init)
    sf_free_list_node *firstnode = sf_add_free_list(size, &sf_free_list_head);

    //set up header of initial block
    char *hp = (char*)sf_mem_start();
    sf_header *header = (sf_header*)(hp + sizeof(sf_prologue));
    initialize_hd_ft(header, 0, 1, 0, size, 0, 1);
    header -> links.prev = &(firstnode -> head);

    //set up footer of initial block
    char *fp = (char*)sf_mem_end();
    sf_footer *footer = (sf_footer*)(fp - (sizeof(sf_epilogue) + sizeof(sf_footer)));
    initialize_hd_ft(footer, 0, 1, 0, size, 0, 1);



}

void initialize_hd_ft(void *item, int al, int prev_al, int zrs, size_t sz, int rq_sz, int flag) {

    if(flag){
        sf_header *header = (sf_header*)item;
        header -> payload = 0;
        header -> info.allocated = al;
        header -> info.prev_allocated = prev_al;
        header -> info.two_zeroes = zrs;
        header -> info.block_size = sz>>4;
        header -> info.requested_size = rq_sz;
    }
    else{
        sf_footer *footer = (sf_footer*)item;
        footer -> info.allocated = al;
        footer -> info.prev_allocated = prev_al;
        footer -> info.two_zeroes = zrs;
        footer -> info.block_size = sz>>4;
        footer -> info.requested_size = rq_sz;
    }
}

void sf_free(void *pp) {
    return;
}

void *sf_realloc(void *pp, size_t rsize) {
    return NULL;
}
