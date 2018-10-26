/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "debug.h"
#include "sfmm.h"
#include <errno.h>

#define LEFT_SHIFT_BLK_SIZE(size) size<<4  //for when extracting block_size
#define RIGHT_SHIFT_BLK_SIZE(size) size>>4 //for when storing block_size

void initialSetup();
void initialize_hd_ft(void *item, int al, int prev_al, int zrs, size_t sz, int rq_sz, int flag);
void insert_block(sf_header *result_block_header, size_t new_block_size);
void circular_linked_list_insert(sf_header *sentinel, sf_header *freeblock);
sf_header *search_freeblock(size_t blk_sz);
sf_free_list_node *search_freelist(size_t blk_sz);
void allocate_block(sf_header *free_block_pointer, size_t size);
void split_block(sf_header *block, size_t rq_sz, size_t blk_sz);
uint64_t *process_request(sf_header *block, size_t rq_sz, size_t blk_sz);
size_t total_block_sz(size_t rq_sz);
sf_header *coalesce_blocks(sf_header *block);
void delete_freeblock_from_freelist(sf_header *header, size_t blk_sz);

int is_allocated_block(sf_header *block);
void split_allocated_block(sf_header *block, size_t rq_sz, size_t blk_sz);

int flag = 1;

void *sf_malloc(size_t size) {

    //do initial setup if first call to sf_malloc
    if(flag){
        initialSetup();
        flag = 0;
    }

    //return if size is 0
    if(size == 0)
        return NULL;

    //compute total block size (minimum is 32 bytes)
    size_t blk_sz = total_block_sz(size);

    //search for free block in appropriate free list node
    sf_header *free_block_pointer = search_freeblock(blk_sz);

    if(free_block_pointer != NULL) {

        //return pointer to first word of payload
        return process_request(free_block_pointer, size, blk_sz);
    }
    else //extend heap, check if need for coalesce, possibly split(if no splinter) and place resulting blocks(maintaining alignment)
    {
        int flag = 1;

        //extend heap
        //create new block
        //move epilogue
        //coalesce if there is a bit of free memory on the heap prior to heap extension
        //check if size is enough for request
            //if yes, split and place resulting free block
            //if no, repeat from the top

        while(flag) {

            char *new_page_start = (char*)sf_mem_grow();         //get address of new page start
            //printf("new page start:%p\n\n", new_page_start);

            if(new_page_start != NULL) {

                char *new_page_hp = new_page_start - sizeof(sf_epilogue);   //get the address of epilogue before new page
                sf_header *new_page_header = (sf_header*)new_page_hp;    //create header for new page

                char *prev_fp = new_page_hp - sizeof(sf_footer);   //get address of previous block's footer
                sf_footer *prev_footer = (sf_footer*)prev_fp;     //get the previous block's footer to check if its allocated

                //set up header for new block
                size_t size_of_new_block = PAGE_SZ;
                initialize_hd_ft(new_page_header, 0, prev_footer -> info.allocated, 0, size_of_new_block, 0, 1);

                char *new_page_end = (char*)sf_mem_end();
                char *eplg_p = new_page_end - sizeof(sf_epilogue);
                char *fp = eplg_p - sizeof(sf_footer);

                //set up new epilogue
                sf_epilogue *eplg = (sf_epilogue*)eplg_p;
                initialize_hd_ft(eplg, 1, 0, 0, 0, 0, 4);

                //set up footer for new block
                sf_footer *new_page_footer = (sf_footer*)fp;
                initialize_hd_ft(new_page_footer, 0, prev_footer -> info.allocated, 0, size_of_new_block, 0, 2);

                sf_header *coalesced_block = coalesce_blocks(new_page_header);

                if(LEFT_SHIFT_BLK_SIZE(coalesced_block -> info.block_size) >= blk_sz) {

                    //return pointer to first word of payload
                    return process_request(coalesced_block, size, blk_sz);

                }
            }
            else  //return NULL
            {
                sf_errno = ENOMEM;
                return NULL;
            }
        }
    }

    return NULL;
}

uint64_t *process_request(sf_header *block, size_t rq_sz, size_t blk_sz) {

    if(LEFT_SHIFT_BLK_SIZE(block -> info.block_size) == blk_sz) {

        allocate_block(block, rq_sz);
    }
    else //possibly split(if no splinter) and place resulting free block(maintaining alignment)
    {
        //split will cause splinter so no splitting
        if(((LEFT_SHIFT_BLK_SIZE(block -> info.block_size)) - blk_sz) < 32) {

            allocate_block(block, rq_sz);
        }
        else //split and place resulting block(maintaining alignment)
        {
            split_block(block, rq_sz, blk_sz);
        }
    }

    return &(block -> payload);
}

sf_header *coalesce_blocks(sf_header *block) {

    char *hp_start = (char*)block;
    char *fp_end = (hp_start + (LEFT_SHIFT_BLK_SIZE(block -> info.block_size)));

    char *prlg_end = (char*)sf_mem_start() + sizeof(sf_prologue);
    char *eplg_start = (char*)sf_mem_end() - sizeof(sf_epilogue);

    char *prev_fp;
    char *next_hp;

    sf_header *temp_nxt_hd;

    int coalesce_up_flag = 0;
    int coalesce_down_flag = 0;

    //possible upward coalesce
    if((fp_end == eplg_start)) {    //if the epilogue is below the footer -> possible upward coalesce
        coalesce_up_flag = 1;
    }
    else {
        temp_nxt_hd = (sf_header*)fp_end;
        if((temp_nxt_hd -> info.allocated == 1)) {    //if the block after is allocated -> possible upward coalesce
            coalesce_up_flag = 1;
        }
    }

    if(coalesce_up_flag) {

        if((hp_start != prlg_end)) {

            //if prev allocated bit is 0 we need to coalesce
            if(block -> info.prev_allocated != 1) {

                //get to the address of the previous block's footer to retrieve the size of the previous block
                prev_fp = hp_start - sizeof(sf_footer);
                sf_footer *prev_footer = (sf_footer*)prev_fp;

                //move header pointer to the previous block's header
                hp_start = hp_start - (LEFT_SHIFT_BLK_SIZE(prev_footer -> info.block_size));

                //update the block size of the previous header to be the sum its block size and the next's
                sf_header *prev_header = (sf_header*)hp_start;
                size_t old_blk_sz  = (LEFT_SHIFT_BLK_SIZE(prev_header -> info.block_size));    //save old size to search for the freelist node later
                prev_header -> info.block_size = RIGHT_SHIFT_BLK_SIZE(
                                                    ((LEFT_SHIFT_BLK_SIZE(prev_header -> info.block_size))
                                                +    (LEFT_SHIFT_BLK_SIZE(block -> info.block_size)))
                                                    );

                //update the block size of the block's footer
                fp_end = fp_end - sizeof(sf_footer);
                sf_footer *footer = (sf_footer*)fp_end;
                footer -> info.block_size = RIGHT_SHIFT_BLK_SIZE(LEFT_SHIFT_BLK_SIZE(prev_header -> info.block_size));

                footer ->info.prev_allocated = prev_header -> info.prev_allocated;   //update footer prev_al to match that of header

                //delete free block from old freelist node
                delete_freeblock_from_freelist(prev_header, old_blk_sz);

                //insert coalesced block in appropriate list
                insert_block(prev_header, LEFT_SHIFT_BLK_SIZE(prev_header -> info.block_size));

                return prev_header;
            }
        }
    }

    //possible downward coalesce
    if((hp_start == prlg_end)) {
        coalesce_down_flag = 1;    //if the prologue is above the header -> possible downward coalesce
    }
    else {
        if(block -> info.prev_allocated == 1) {    //if the block before is allocated -> possible downward coalesce
            coalesce_down_flag = 1;
        }
    }

    if(coalesce_down_flag) {

        if((fp_end != eplg_start)) {

            //get to the address of the next block's header to check if its allocated
            next_hp = fp_end;
            sf_header *next_header = (sf_header*)next_hp;

            //if allocated bit of next header is 0 we need to coalesce
            if(next_header -> info.allocated != 1) {

                //move footer_end pointer to the next block's footer
                fp_end = (fp_end + (LEFT_SHIFT_BLK_SIZE(next_header -> info.block_size))) - sizeof(sf_footer);

                //update the block size of the block's header
                sf_header *header = (sf_header*)hp_start;

                header -> info.block_size =  RIGHT_SHIFT_BLK_SIZE(
                                            ( (LEFT_SHIFT_BLK_SIZE(header -> info.block_size))
                                            + (LEFT_SHIFT_BLK_SIZE(next_header -> info.block_size))));

                //update the block size of the next block's footer
                sf_footer *next_footer = (sf_footer*)fp_end;
                size_t old_blk_sz  = (LEFT_SHIFT_BLK_SIZE(next_footer -> info.block_size));    //save old size to search for the freelist node later
                next_footer -> info.block_size = RIGHT_SHIFT_BLK_SIZE(LEFT_SHIFT_BLK_SIZE(header -> info.block_size));

                next_footer -> info.prev_allocated = header -> info.prev_allocated;

                //delete free block from old freelist node
                delete_freeblock_from_freelist(next_header, old_blk_sz);

                insert_block(header, LEFT_SHIFT_BLK_SIZE(header -> info.block_size));

                return header;
            }
        }
    }

    //possible upward/downward coalesce
    if((hp_start != prlg_end) && (fp_end != eplg_start)) {

        //get to the address of the next block's header to check if its allocated
        next_hp = fp_end;
        sf_header *next_header = (sf_header*)next_hp;

        if((block -> info.prev_allocated != 1) && (next_header -> info.allocated != 1)) {

            //get to the address of the previous block's footer to retrieve the size of the previous block
            prev_fp = hp_start - sizeof(sf_footer);
            sf_footer *prev_footer = (sf_footer*)prev_fp;

            sf_header *middle_header = (sf_header*)hp_start;

            //move header pointer to the previous block's header
            hp_start = hp_start - (LEFT_SHIFT_BLK_SIZE(prev_footer -> info.block_size));

            //move footer pointer to the next block's footer
            fp_end = (fp_end + (LEFT_SHIFT_BLK_SIZE(next_header -> info.block_size))) - sizeof(sf_footer);
            sf_footer *next_footer = (sf_footer*)fp_end;

            //update the block size of the previous block's header
            sf_header *prev_header = (sf_header*)hp_start;
            size_t old_blk_sz_prev_hd  = (LEFT_SHIFT_BLK_SIZE(prev_header -> info.block_size));    //save old size to search for the freelist node later
            size_t old_blk_sz_next_ft  = (LEFT_SHIFT_BLK_SIZE(next_footer -> info.block_size));
            prev_header -> info.block_size = RIGHT_SHIFT_BLK_SIZE(
                                            ((LEFT_SHIFT_BLK_SIZE(prev_header -> info.block_size)) +
                                            (LEFT_SHIFT_BLK_SIZE(middle_header -> info.block_size))
                                            + (LEFT_SHIFT_BLK_SIZE(next_footer -> info.block_size))));

            //update the block size of the next block's footer
            next_footer -> info.block_size = RIGHT_SHIFT_BLK_SIZE(LEFT_SHIFT_BLK_SIZE(prev_header -> info.block_size));

            next_footer -> info.prev_allocated = prev_header -> info.prev_allocated;

            //delete free block from old freelist node
            delete_freeblock_from_freelist(prev_header, old_blk_sz_prev_hd);
            delete_freeblock_from_freelist(next_header, old_blk_sz_next_ft);

            insert_block(prev_header, LEFT_SHIFT_BLK_SIZE(prev_header -> info.block_size));

            return prev_header;
        }
    }

    //no coalesce
    insert_block(block, LEFT_SHIFT_BLK_SIZE(block -> info.block_size));
    return block;
}

void delete_freeblock_from_freelist(sf_header *header, size_t blk_sz) {

    sf_free_list_node *node = search_freelist(blk_sz);

    //if not the only free block in the list
    if((header -> links.next) != &(node -> head)) {
        node -> head.links.next = header -> links.next;

    }
    else {
        node -> head.links.prev = &(node -> head);
        node -> head.links.next = &(node -> head);
    }
}

void split_block(sf_header *block, size_t rq_sz, size_t blk_sz) {

    char *hp = (char*)block;

    hp = hp + blk_sz;

    //ensure cut off address is properly alligned

    while( !(((uintptr_t)hp % 8 == 0) && ((((uintptr_t)hp/8)%2) == 1)) ) {
        hp++;
        blk_sz++;
    }

    size_t new_block_size = (LEFT_SHIFT_BLK_SIZE(block -> info.block_size)) - blk_sz;

    //if new_block_size is a splinter, split is not done any longer
    if(new_block_size < 32) {

        allocate_block(block, rq_sz);
    }
    else
    {
        //set up header and footer of new block
        sf_header *result_block_header = (sf_header*)hp;
        initialize_hd_ft(result_block_header, 0, 1, 0, new_block_size, 0, 1);

        char *fp = (char*)block;

        fp = (fp + (LEFT_SHIFT_BLK_SIZE(block -> info.block_size))) - sizeof(sf_footer);

        sf_footer *result_block_footer = (sf_footer*)fp;
        initialize_hd_ft(result_block_footer, 0, 1, 0, new_block_size, 0, 2);

        //allocate the split block and set the new blocksize
        allocate_block(block, rq_sz);
        block -> info.block_size = RIGHT_SHIFT_BLK_SIZE(blk_sz);

        //insert_block(result_block_header, new_block_size);

        //coalesce if need be, and insert afterwards(done in coalesce_blocks)
        coalesce_blocks(result_block_header);
    }
}

void insert_block(sf_header *result_block_header, size_t block_size) {

    //find a freelist of size >= the block's size
    sf_free_list_node *result_block_freelist = search_freelist(block_size);

    //if a list is not found, add a new sf_free_list_node for that size using sf_free_list_head as 'next'
    //then insert the block in that list
    if(result_block_freelist == NULL) {
        sf_free_list_node *freelist = sf_add_free_list(block_size, &sf_free_list_head);
        circular_linked_list_insert(&(freelist -> head), result_block_header);
    }
     else
    {
        //if the list's size = new_block_size, insert block in that list
        if(result_block_freelist -> size == block_size) {
            circular_linked_list_insert(&(result_block_freelist -> head), result_block_header);
        }
        //else add a new sf_free_list_node for that size using result_block_freelist as 'next'
        //then insert the block in that list
        else
        {
            sf_free_list_node *freelist = sf_add_free_list(block_size, result_block_freelist);
            circular_linked_list_insert(&(freelist -> head), result_block_header);
        }
    }
}

void circular_linked_list_insert(sf_header *sentinel, sf_header *freeblock) {

    if(sentinel -> links.next == sentinel) {
        sentinel -> links.next = freeblock;
        sentinel -> links.prev = freeblock;
        freeblock -> links.prev = sentinel;
        freeblock -> links.next = sentinel;
    }
    else {
        freeblock -> links.next = sentinel -> links.next;
        freeblock -> links.prev = sentinel;
        sentinel -> links.next -> links.prev = freeblock;
        sentinel -> links.next = freeblock;
    }
}

void allocate_block(sf_header *free_block_pointer, size_t size) {

    //if its not the only block in the list
    if(free_block_pointer -> links.next != free_block_pointer -> links.prev) {
        //set sentinel's next to its next block
        free_block_pointer -> links.prev -> links.next = free_block_pointer -> links.next;
        //set its next block's prev to sentinel
        free_block_pointer -> links.next -> links.prev = free_block_pointer -> links.prev;
    }
    else //else set the prev and next of the sentinel to itself
    {
        free_block_pointer -> links.prev -> links.next = free_block_pointer -> links.prev;
        free_block_pointer -> links.prev -> links.prev = free_block_pointer -> links.prev;
    }

        //set allocated, rq_sz, blk_sz, next, prev of block to be allocated
        free_block_pointer -> info.allocated = 1;
        free_block_pointer -> info.requested_size = size;
        //free_block_pointer -> info.block_size = blk_sz>>4;
        free_block_pointer -> links.next = NULL;
        free_block_pointer -> links.prev = NULL;

        //set prev_allocated of the next block on heap(both in header and footer|only footer if next block is epilogue)
        char *fp = (char*)free_block_pointer;
        fp = fp + (LEFT_SHIFT_BLK_SIZE(free_block_pointer -> info.block_size));

        //if next block is epilogue
        if((fp + sizeof(sf_epilogue)) == (char*)sf_mem_end()) {
            // sf_epilogue *eplg = (sf_epilogue*)fp;
            // eplg -> footer.info.prev_allocated = 1;
        }
        else //else next block is a regular memory block
        {
            sf_header *hd = (sf_header*)fp;
            hd -> info.prev_allocated = 1;
            fp = (fp + (LEFT_SHIFT_BLK_SIZE(hd -> info.block_size))) - sizeof(sf_footer);
            sf_footer *ft = (sf_footer*)fp;
            ft -> info.prev_allocated = 1;
        }
}

sf_free_list_node *search_freelist(size_t blk_sz) {

    sf_free_list_node *next = sf_free_list_head.next;

    while((next != &sf_free_list_head)) {

        if((next -> size >= blk_sz))
            return next;

        next = next -> next;
    }
    return NULL;
}

sf_header *search_freeblock(size_t blk_sz) {

    sf_free_list_node *next = sf_free_list_head.next;

    while((next != &sf_free_list_head)) {

        if((next -> size >= blk_sz) && (next -> head.links.next != &(next->head)))
            return next -> head.links.next;

        next = next -> next;
    }
    return NULL;
}

size_t total_block_sz(size_t rq_sz) {

    size_t total = rq_sz + sizeof(sf_block_info);

    if(total < 32)
        return 32;

    while((total % 16 != 0))
        total++;

    return total;
}

void initialSetup() {

    sf_mem_grow();

    //set up prologue
    sf_prologue *prlg = (sf_prologue*)sf_mem_start();
    initialize_hd_ft(prlg, 1, 0, 0, 0, 0, 3);

    //set up epilogue
    char *fp = (char*)sf_mem_end();
    fp = (fp - sizeof(sf_epilogue));
    sf_epilogue *eplg = (sf_epilogue*)fp;
    initialize_hd_ft(eplg, 1, 0, 0, 0, 0, 4);

    //size of initial block
    size_t size = PAGE_SZ - (sizeof(sf_prologue) + sizeof(sf_epilogue));

    //create first sf_free_list_node after sentinel (created by sf_mem_init)
    sf_free_list_node *firstnode = sf_add_free_list(size, &sf_free_list_head);

    //set up header of initial block
    char *hp = (char*)sf_mem_start();
    hp = hp + sizeof(sf_prologue);
    sf_header *header = (sf_header*)hp;
    initialize_hd_ft(header, 0, 1, 0, size, 0, 1);

    //maintain circular linked list
    header -> links.prev = &(firstnode -> head);
    header -> links.next = &(firstnode -> head);
    firstnode -> head.links.next = header;
    firstnode -> head.links.prev = header;

    // char *p = (char*)&(firstnode -> head);
    // printf("sentinel address:%p\n", p);

    //set up footer of initial block
    fp = (char*)sf_mem_end();
    fp = (fp - (sizeof(sf_epilogue) + sizeof(sf_footer)));
    sf_footer *footer = (sf_footer*)fp;
    initialize_hd_ft(footer, 0, 1, 0, size, 0, 2);
}

void initialize_hd_ft(void *item, int al, int prev_al, int zrs, size_t sz, int rq_sz, int flag) {

    if((flag == 1)) {
        sf_header *header = (sf_header*)item;
        header -> payload = 0;
        header -> info.allocated = al;
        header -> info.prev_allocated = prev_al;
        header -> info.two_zeroes = zrs;
        header -> info.block_size = sz>>4;
        header -> info.requested_size = rq_sz;
    }
    else if((flag == 2)) {
        sf_footer *footer = (sf_footer*)item;
        footer -> info.allocated = al;
        footer -> info.prev_allocated = prev_al;
        footer -> info.two_zeroes = zrs;
        footer -> info.block_size = sz>>4;
        footer -> info.requested_size = rq_sz;
    }
    else if((flag == 3)) {
        sf_prologue *prlg = (sf_prologue*)item;
        prlg -> padding = 0;
        prlg -> header.payload = 0;
        prlg -> header.info.allocated = al;
        prlg -> header.info.prev_allocated = prev_al;
        prlg -> header.info.two_zeroes = zrs;
        prlg -> header.info.block_size = sz>>4;
        prlg -> header.info.requested_size = rq_sz;

        prlg -> footer.info.allocated = al;
        prlg -> footer.info.prev_allocated = prev_al;
        prlg -> footer.info.two_zeroes = zrs;
        prlg -> footer.info.block_size = sz>>4;
        prlg -> footer.info.requested_size = rq_sz;

    }
    else if((flag == 4)) {
        sf_epilogue *eplg = (sf_epilogue*)item;
        eplg -> footer.info.allocated = al;
        eplg -> footer.info.prev_allocated = prev_al;
        eplg -> footer.info.two_zeroes = zrs;
        eplg -> footer.info.block_size = sz>>4;
        eplg -> footer.info.requested_size = rq_sz;

    }

}

void sf_free(void *pp) {

    char *hp = pp - sizeof(sf_block_info);

    sf_header *header = (sf_header*)hp;

    if(is_allocated_block(header)) {

        header -> info.allocated = 0;
        header -> info.requested_size = 0;

        char *fp = (hp + (LEFT_SHIFT_BLK_SIZE(header -> info.block_size))) - sizeof(sf_footer);
        sf_footer *footer = (sf_footer*)fp;
        footer -> info.prev_allocated = header -> info.prev_allocated;
        footer -> info.block_size = RIGHT_SHIFT_BLK_SIZE(LEFT_SHIFT_BLK_SIZE(header -> info.block_size));

        char *prev_hp = hp + (LEFT_SHIFT_BLK_SIZE(header -> info.block_size));  //get possible next header pointer

        char *eplg_p = ((char*)sf_mem_end()) - sizeof(sf_epilogue);  //get eplg pointer

        //check if it is the eplg instead of an actual block: if there an actual block below this block to be freed, set that
        //blocks header and footer's prev_allocated to 0
        if(prev_hp != eplg_p) {

            sf_header *prev_hd = (sf_header*)prev_hp;

            char *prev_fp = (prev_hp + (LEFT_SHIFT_BLK_SIZE(prev_hd -> info.block_size))) - sizeof(sf_footer);

            sf_footer *prev_ft = (sf_footer*)prev_fp;

            prev_hd -> info.prev_allocated = 0;
            prev_ft -> info.prev_allocated = 0;

        }

        coalesce_blocks(header);
    }
    else {
        abort();
    }

    return;
}

int is_allocated_block(sf_header *block) {

    //if pointer is NULL
    if(block == NULL)
        return 0;

    char *prlg_start = (char*)sf_mem_start();
    char *prlg_end = prlg_start + sizeof(sf_prologue); //pointer to end of prlg
    char *eplg_end = (char*)sf_mem_end();
    char *eplg_start = eplg_end - sizeof(sf_epilogue); //pointer to start of eplg

    char *block_hp = (char*)block;

    //if the header of the block is before the end of the prologue or after the beginning of the epilogue
    if((block_hp < prlg_end) || block_hp > eplg_start)
        return 0;

    //char *block_fp = (block_hp + (LEFT_SHIFT_BLK_SIZE(block -> info.block_size))) - sizeof(sf_footer);

    //sf_footer *block_footer = (sf_footer*)block_fp;

    //if the allocated bit in the header or footer is 0
    if((block -> info.allocated == 0))
        return 0;

    size_t blk_sz = LEFT_SHIFT_BLK_SIZE(block -> info.block_size);

    //if the block_size field is not a multiple of 16 or is less than the minimum block size of 32 bytes
    if((blk_sz % 16 != 0) || (blk_sz < 32))
        return 0;

    size_t sum_rq_sz_hd_sz = (block -> info.requested_size) + sizeof(sf_block_info);

    //if the requested_size field, plus the size required for the block header, is greater than the block_size field.
    if(sum_rq_sz_hd_sz > (LEFT_SHIFT_BLK_SIZE(block -> info.block_size)))
        return 0;

    if(block -> info.prev_allocated == 0) {

        char *prev_fp = block_hp - sizeof(sf_footer);   //previous footer pointer
        sf_footer *prev_footer = (sf_footer*)prev_fp;   //previous footer

        char *prev_hp = block_hp - (LEFT_SHIFT_BLK_SIZE(prev_footer -> info.block_size));
        sf_header *prev_header = (sf_header*)prev_hp;

        //If the prev_alloc field is 0, indicating that the previous block is free, then the alloc fields of the previous block header and footer should also be 0
        if((prev_header -> info.allocated != 0) || (prev_footer -> info.allocated != 0))
            return 0;
    }

    return 1;
}

void *sf_realloc(void *pp, size_t rsize) {

    char *hp = pp - sizeof(sf_block_info);

    sf_header *header = (sf_header*)hp;

    //return NULL if invalid pointer
    if(!is_allocated_block(header)) {
        sf_errno = EINVAL;
        //abort();
        return NULL;
    }

    //if rsize = 0, free the block and return NULL
    if(rsize == 0) {

        sf_free(pp);
        return NULL;
    }

    //if rsize is > than the block size, we reallocate to a larger size
    if(rsize > LEFT_SHIFT_BLK_SIZE(header -> info.block_size)) {

        void *larger_block = sf_malloc(rsize);   //larger block's payload pointer

        //if sf_malloc returns NULL, return NULL
        if(larger_block == NULL)
            return NULL;

        void *header_payload_p = (void*)(&(header -> payload));   //original payload pointer
        size_t bytes_to_copy = header -> info.requested_size;

        memcpy(larger_block, header_payload_p, bytes_to_copy);

        sf_free(pp);

        return larger_block;

    }
    else //we reallocate to a smaller size
    {
        size_t blk_sz = total_block_sz(rsize);
        split_allocated_block(header, rsize, blk_sz);

        return (void*)&(header -> payload);
    }

    return NULL;
}

void split_allocated_block(sf_header *block, size_t rq_sz, size_t blk_sz) {

    char *hp = (char*)block;

    hp = hp + blk_sz;

    //ensure cut off address is properly alligned

    while( !(((uintptr_t)hp % 8 == 0) && ((((uintptr_t)hp/8)%2) == 1)) ) {
        hp++;
        blk_sz++;
    }

    size_t new_block_size = (LEFT_SHIFT_BLK_SIZE(block -> info.block_size)) - blk_sz;

    //if new_block_size is a splinter, split is not done any longer
    if(new_block_size < 32) {

        //set header and footer requested size fields to smaller size
        block -> info.requested_size = rq_sz;
        // char *fp = hp + (LEFT_SHIFT_BLK_SIZE(block -> info.block_size));
        // sf_footer *footer = (sf_footer*)fp;
        // footer -> info.requested_size = rq_sz;
    }
    else
    {
        //set up header and footer of new block
        sf_header *result_block_header = (sf_header*)hp;
        initialize_hd_ft(result_block_header, 0, 1, 0, new_block_size, 0, 1);

        char *fp = (char*)block;

        fp = (fp + (LEFT_SHIFT_BLK_SIZE(block -> info.block_size))) - sizeof(sf_footer);

        sf_footer *result_block_footer = (sf_footer*)fp;
        initialize_hd_ft(result_block_footer, 0, 1, 0, new_block_size, 0, 2);

        //set the new blocksize and rq_sz of the split block
        block -> info.block_size = RIGHT_SHIFT_BLK_SIZE(blk_sz);
        block -> info.requested_size = rq_sz;

        //coalesce if need be, and insert afterwards(done in coalesce_blocks)
        coalesce_blocks(result_block_header);

    }
}
