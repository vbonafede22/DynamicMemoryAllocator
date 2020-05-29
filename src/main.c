#include <stdio.h>
//#include "__grading_helpers.h"
#include "sfmm.h"

#define ALIGN_SHIFT               4U
#define ALIGN(v)                  ((((v) + (1U << ALIGN_SHIFT) - 1U) >> ALIGN_SHIFT) << ALIGN_SHIFT)
#define MIN_BLOCK_SIZE            ((size_t) (ALIGN(sizeof(sf_block))))
#define PAYLOAD_TO_BLOCK(pp)      ((sf_block *) ((char *) (pp) - (char *) &(((sf_block *) 0x0)->body.payload)))
#define ALLOCATED(bp)             (((bp) -> header & THIS_BLOCK_ALLOCATED) != 0)


int main(int argc, char const *argv[]) {
    sf_mem_init();

    //double* ptr = sf_malloc(sizeof(double));

    //*ptr = 320320320e-320;

    //printf("pointer: %f\n", *ptr);

    //sf_free(ptr);

    // double* ptr = sf_malloc(4);
    // ptr = sf_malloc(200);
    // ptr = sf_malloc(18);
    // ptr = sf_malloc(40);
    // ptr = sf_malloc(1000);
    // ptr = sf_malloc(1000);
    // ptr = sf_malloc(2000);        //this will cause a call to sf grow 
    // //ptr = ptr;

    // *ptr = 6;

    // printf("pointer: %f\n", *ptr);      //printing 0's not ok?

    // //sf_show_heap();

    // sf_free(ptr);


    // void *y = sf_malloc(8);
    // sf_malloc(1);
    // void *z =  sf_malloc(1);

    // sf_free(y);
    // // sf_free(x);
    // sf_free(z);

    // sf_show_heap();
    
    // sf_mem_fini();

    size_t sz = 1;
    //size_t nsz = 1020;
    // size_t sz2 = PAGE_SZ - 224 - sizeof(sf_prologue) - sizeof(sf_epilogue) - (1 << ALIGN_SHIFT);
    // //size_t sz3 = 400;
    // //size_t sz4 = 500;

    void * x = sf_malloc(sz);
    //printf("block: %p\n", bp);
    printf("alloc bit: %d\n", ALLOCATED(PAYLOAD_TO_BLOCK(x)));
    //sf_malloc(sz);
    // sf_malloc(sz1);
    // void * y = sf_malloc(sz1);
    // sf_malloc(sz1);
    // sf_malloc(sz1);
    // void * z = sf_malloc(sz1);
    // sf_malloc(sz1);

    sf_show_heap();
    //printf("%ld\n", PAGE_SZ * 2 - sizeof(sf_epilogue) - sizeof(sf_prologue) - 4064);
    
    sf_mem_fini();

    return EXIT_SUCCESS;
}
