/**
 * grading_tests_p2.c
 * @brief      part 1 for hw3 grading test cases
 *             include grading_tests_p2.c edited by Xi Han when conducting tests
 * @editor     Linh Pham
 * @edited     Nov 7 2019
 * @revisor    Xi Han
 * @revised    Nov 10 2019
 *
 */

#include "__grading_helpers.h"

/*
 * Do one malloc and check that the prologue and epilogue are correctly initialized
 */
Test(sf_memsuite_grading, initialization, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz = 1;
    (void) sf_malloc(sz);

    _assert_prologue_is_valid();
    _assert_epilogue_is_valid();
    _assert_heap_is_valid();
}

/*
 * Single malloc tests, up to the size that forces a non-minimum block size.
 */
Test(sf_memsuite_grading, single_malloc_1, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz = 1;
    void * x = sf_malloc(sz);
    _assert_nonnull_payload_pointer(x);     //pass
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz));
    _assert_heap_is_valid();    //pass
    
    size_t exp_free_sz = PAGE_SZ - sizeof(sf_epilogue) - sizeof(sf_prologue) - ADJUSTED_BLOCK_SIZE(sz);
    _assert_free_block_count(exp_free_sz, 1);   //pass

    _assert_errno_eq(0);
}

Test(sf_memsuite_grading, single_malloc_4, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz = 4;
    void * x = sf_malloc(sz);
    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz));
    _assert_heap_is_valid();

    size_t exp_free_sz = PAGE_SZ - sizeof(sf_epilogue) - sizeof(sf_prologue) - ADJUSTED_BLOCK_SIZE(sz);
    _assert_free_block_count(exp_free_sz, 1);

    _assert_errno_eq(0);
}

Test(sf_memsuite_grading, single_malloc_8, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz = 8;
    void * x = sf_malloc(sz);
    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz));
    _assert_heap_is_valid();

    size_t exp_free_sz = PAGE_SZ - sizeof(sf_epilogue) - sizeof(sf_prologue) - ADJUSTED_BLOCK_SIZE(sz);
    _assert_free_block_count(exp_free_sz, 1);

    _assert_errno_eq(0);
}

Test(sf_memsuite_grading, single_malloc_16, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz = 16;
    void * x = sf_malloc(sz);
    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz));
    _assert_heap_is_valid();

    size_t exp_free_sz = PAGE_SZ - sizeof(sf_epilogue) - sizeof(sf_prologue) - ADJUSTED_BLOCK_SIZE(sz);
    _assert_free_block_count(exp_free_sz, 1);

    _assert_errno_eq(0);
}

Test(sf_memsuite_grading, single_malloc_32, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz = 32;
    void * x = sf_malloc(sz);
    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz));
    _assert_heap_is_valid();

    size_t exp_free_sz = PAGE_SZ - sizeof(sf_epilogue) - sizeof(sf_prologue) - ADJUSTED_BLOCK_SIZE(sz);
    _assert_free_block_count(exp_free_sz, 1);

    _assert_errno_eq(0);
}


/*
 * Single malloc test, of a size exactly equal to what is left after initialization.
 */
Test(sf_memsuite_grading, single_malloc_4024, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz = PAGE_SZ - sizeof(sf_epilogue) - sizeof(sf_prologue)
	                - (sizeof(sf_block) - sizeof(sf_header));
    void * x = sf_malloc(sz);
    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz));
    _assert_heap_is_valid();

    _assert_free_block_count(0, 0);

    _assert_errno_eq(0);
}

/*
 * Single malloc test, of a size just larger than what is left after initialization.
 */
Test(sf_memsuite_grading, single_malloc_4040, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz = PAGE_SZ - sizeof(sf_epilogue) - sizeof(sf_prologue)
	                - (sizeof(sf_block) - sizeof(sf_header)) + (1 << ALIGN_SHIFT);
    void * x = sf_malloc(sz);
    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz));
    _assert_heap_is_valid();

    size_t exp_free_sz = PAGE_SZ * 2 - sizeof(sf_epilogue) - sizeof(sf_prologue) - ADJUSTED_BLOCK_SIZE(sz);
    _assert_free_block_count(exp_free_sz, 1);

    _assert_errno_eq(0);
}

/*
 * Single malloc test, of multiple pages.
 */
Test(sf_memsuite_grading, single_malloc_12000, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz = 12000;
    void * x = sf_malloc(sz);
    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz));
    _assert_heap_is_valid();

    size_t exp_free_sz = PAGE_SZ * 3 - sizeof(sf_epilogue) - sizeof(sf_prologue) - ADJUSTED_BLOCK_SIZE(sz);
    _assert_free_block_count(exp_free_sz, 1);

    _assert_errno_eq(0);
}

/*
 * Single malloc test, unsatisfiable.
 * There should be left one single large block.
 */
Test(sf_memsuite_grading, single_malloc_max, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz = MAX_SIZE;
    void * x = sf_malloc(sz);
    _assert_null_payload_pointer(x);
    _assert_heap_is_valid();

    size_t exp_free_sz = MAX_SIZE - sizeof(sf_epilogue) - sizeof(sf_prologue);
    _assert_free_block_count(exp_free_sz, 1);

    _assert_errno_eq(ENOMEM);
}

/*
 * Malloc/free with/without coalescing.
 */
Test(sf_memsuite_grading, malloc_free_no_coalesce, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz1 = 200;
    size_t sz2 = 300;
    size_t sz3 = 400;

    void * x = sf_malloc(sz1);
    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz1));

    void * y = sf_malloc(sz2);
    _assert_nonnull_payload_pointer(y);
    _assert_block_info(PAYLOAD_TO_BLOCK(y), 1, ADJUSTED_BLOCK_SIZE(sz2));

    void * z = sf_malloc(sz3);
    _assert_nonnull_payload_pointer(z);
    _assert_block_info(PAYLOAD_TO_BLOCK(z), 1, ADJUSTED_BLOCK_SIZE(sz3));

    sf_free(y);

    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz1));
    _assert_block_info(PAYLOAD_TO_BLOCK(y), 0, ADJUSTED_BLOCK_SIZE(sz2));
    _assert_block_info(PAYLOAD_TO_BLOCK(z), 1, ADJUSTED_BLOCK_SIZE(sz3));
    _assert_heap_is_valid();

    _assert_free_block_count(ADJUSTED_BLOCK_SIZE(sz2), 1);

    size_t exp_free_2_sz = PAGE_SZ -
                           ADJUSTED_BLOCK_SIZE(sz1) - ADJUSTED_BLOCK_SIZE(sz2) - ADJUSTED_BLOCK_SIZE(sz3) -
                           sizeof(sf_prologue) - sizeof(sf_epilogue);
    _assert_free_block_count(exp_free_2_sz, 1);

    _assert_errno_eq(0);
}

Test(sf_memsuite_grading, malloc_free_coalesce_lower, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz1 = 200;
    size_t sz2 = 300;
    size_t sz3 = 400;
    size_t sz4 = 500;

    void * x = sf_malloc(sz1);
    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz1));

    void * y = sf_malloc(sz2);
    _assert_nonnull_payload_pointer(y);
    _assert_block_info(PAYLOAD_TO_BLOCK(y), 1, ADJUSTED_BLOCK_SIZE(sz2));

    void * z = sf_malloc(sz3);
    _assert_nonnull_payload_pointer(z);
    _assert_block_info(PAYLOAD_TO_BLOCK(z), 1, ADJUSTED_BLOCK_SIZE(sz3));

    void * w = sf_malloc(sz4);
    _assert_nonnull_payload_pointer(w);
    _assert_block_info(PAYLOAD_TO_BLOCK(w), 1, ADJUSTED_BLOCK_SIZE(sz4));

    sf_free(y);
    sf_free(z);

    size_t sz = ADJUSTED_BLOCK_SIZE(sz2) + ADJUSTED_BLOCK_SIZE(sz3);

    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz1));
    _assert_block_info(PAYLOAD_TO_BLOCK(y), 0, sz);
    _assert_block_info(PAYLOAD_TO_BLOCK(w), 1, ADJUSTED_BLOCK_SIZE(sz4));
    _assert_heap_is_valid();

    _assert_free_block_count(ADJUSTED_BLOCK_SIZE(sz2) + ADJUSTED_BLOCK_SIZE(sz3), 1);

    size_t exp_free_2_sz = PAGE_SZ -
                           ADJUSTED_BLOCK_SIZE(sz1) - ADJUSTED_BLOCK_SIZE(sz2) -
                           ADJUSTED_BLOCK_SIZE(sz3) - ADJUSTED_BLOCK_SIZE(sz4) -
                           sizeof(sf_prologue) - sizeof(sf_epilogue);
    _assert_free_block_count(exp_free_2_sz, 1);

    _assert_errno_eq(0);
}

Test(sf_memsuite_grading, malloc_free_coalesce_upper, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz1 = 200;
    size_t sz2 = 300;
    size_t sz3 = 400;
    size_t sz4 = 500;

    void * x = sf_malloc(sz1);
    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz1));

    void * y = sf_malloc(sz2);
    _assert_nonnull_payload_pointer(y);
    _assert_block_info(PAYLOAD_TO_BLOCK(y), 1, ADJUSTED_BLOCK_SIZE(sz2));

    void * z = sf_malloc(sz3);
    _assert_nonnull_payload_pointer(z);
    _assert_block_info(PAYLOAD_TO_BLOCK(z), 1, ADJUSTED_BLOCK_SIZE(sz3));

    void * w = sf_malloc(sz4);
    _assert_nonnull_payload_pointer(w);
    _assert_block_info(PAYLOAD_TO_BLOCK(w), 1, ADJUSTED_BLOCK_SIZE(sz4));

    sf_free(z);
    sf_free(y);
    size_t sz = ADJUSTED_BLOCK_SIZE(sz2) + ADJUSTED_BLOCK_SIZE(sz3);

    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz1));
    _assert_block_info(PAYLOAD_TO_BLOCK(y), 0, sz);
    _assert_block_info(PAYLOAD_TO_BLOCK(w), 1, ADJUSTED_BLOCK_SIZE(sz4));
    _assert_heap_is_valid();

    _assert_free_block_count(ADJUSTED_BLOCK_SIZE(sz2) + ADJUSTED_BLOCK_SIZE(sz3), 1);

    size_t exp_free_2_sz = PAGE_SZ -
                           ADJUSTED_BLOCK_SIZE(sz1) - ADJUSTED_BLOCK_SIZE(sz2) -
                           ADJUSTED_BLOCK_SIZE(sz3) - ADJUSTED_BLOCK_SIZE(sz4) -
                           sizeof(sf_prologue) - sizeof(sf_epilogue);
    _assert_free_block_count(exp_free_2_sz, 1);

    _assert_errno_eq(0);
}

Test(sf_memsuite_grading, malloc_free_coalesce_both, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz1 = 200;
    size_t sz2 = 300;
    size_t sz3 = 400;
    size_t sz4 = 500;

    void * x = sf_malloc(sz1);
    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz1));

    void * y = sf_malloc(sz2);
    _assert_nonnull_payload_pointer(y);
    _assert_block_info(PAYLOAD_TO_BLOCK(y), 1, ADJUSTED_BLOCK_SIZE(sz2));

    void * z = sf_malloc(sz3);
    _assert_nonnull_payload_pointer(z);
    _assert_block_info(PAYLOAD_TO_BLOCK(z), 1, ADJUSTED_BLOCK_SIZE(sz3));

    void * w = sf_malloc(sz4);
    _assert_nonnull_payload_pointer(w);
    _assert_block_info(PAYLOAD_TO_BLOCK(w), 1, ADJUSTED_BLOCK_SIZE(sz4));

    sf_free(x);
    sf_free(z);
    sf_free(y);
    size_t sz = ADJUSTED_BLOCK_SIZE(sz1) + ADJUSTED_BLOCK_SIZE(sz2) + ADJUSTED_BLOCK_SIZE(sz3);

    _assert_block_info(PAYLOAD_TO_BLOCK(x), 0, sz);
    _assert_heap_is_valid();

    _assert_free_block_count(ADJUSTED_BLOCK_SIZE(sz1) + ADJUSTED_BLOCK_SIZE(sz2) + ADJUSTED_BLOCK_SIZE(sz3), 1);

    size_t exp_free_2_sz = PAGE_SZ -
                           ADJUSTED_BLOCK_SIZE(sz1) - ADJUSTED_BLOCK_SIZE(sz2) -
                           ADJUSTED_BLOCK_SIZE(sz3) - ADJUSTED_BLOCK_SIZE(sz4) -
                           sizeof(sf_prologue) - sizeof(sf_epilogue);
    _assert_free_block_count(exp_free_2_sz, 1);

    _assert_errno_eq(0);
}

Test(sf_memsuite_grading, malloc_free_first_block, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz1 = 200;
    size_t sz2 = 300;

    void * x = sf_malloc(sz1);
    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz1));

    void * y = sf_malloc(sz2);
    _assert_nonnull_payload_pointer(y);
    _assert_block_info(PAYLOAD_TO_BLOCK(y), 1, ADJUSTED_BLOCK_SIZE(sz2));

    sf_free(x);

    _assert_block_info(PAYLOAD_TO_BLOCK(x), 0, ADJUSTED_BLOCK_SIZE(sz1));
    _assert_block_info(PAYLOAD_TO_BLOCK(y), 1, ADJUSTED_BLOCK_SIZE(sz2));
    _assert_heap_is_valid();

    _assert_free_block_count(ADJUSTED_BLOCK_SIZE(sz1), 1);

    size_t exp_free_2_sz = PAGE_SZ -
                           ADJUSTED_BLOCK_SIZE(sz1) - ADJUSTED_BLOCK_SIZE(sz2) -
                           sizeof(sf_prologue) - sizeof(sf_epilogue);
    _assert_free_block_count(exp_free_2_sz, 1);

    _assert_errno_eq(0);
}

Test(sf_memsuite_grading, malloc_free_last_block, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz1 = 200;
    size_t sz2 = PAGE_SZ - ADJUSTED_BLOCK_SIZE(sz1)
	                 - sizeof(sf_prologue) - sizeof(sf_epilogue) - (1 << ALIGN_SHIFT);

    void * x = sf_malloc(sz1);
    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz1));

    void * y = sf_malloc(sz2);
    _assert_nonnull_payload_pointer(y);
    _assert_block_info(PAYLOAD_TO_BLOCK(y), 1, ADJUSTED_BLOCK_SIZE(sz2));

    sf_free(y);

    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz1));

    size_t exp_free_sz = PAGE_SZ - ADJUSTED_BLOCK_SIZE(sz1) - sizeof(sf_prologue) - sizeof(sf_epilogue);
    _assert_block_info(PAYLOAD_TO_BLOCK(y), 0, exp_free_sz);
    _assert_free_block_count(exp_free_sz, 1);

    _assert_heap_is_valid();

    _assert_errno_eq(0);
}

/*
 * Check that malloc leaves no splinter.
 */
Test(sf_memsuite_grading, malloc_with_splinter, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz = PAGE_SZ - sizeof(sf_prologue) - sizeof(sf_epilogue) - MIN_BLOCK_SIZE;
    void * x = sf_malloc(sz);

    _assert_nonnull_payload_pointer(x);
    _assert_block_info(PAYLOAD_TO_BLOCK(x), 1, ADJUSTED_BLOCK_SIZE(sz) + (1 << ALIGN_SHIFT));
    _assert_heap_is_valid();

    _assert_free_block_count(0, 0);

    _assert_errno_eq(0);
}

/*
 *  Allocate small blocks until memory exhausted.
 */
Test(sf_memsuite_grading, malloc_to_exhaustion, .init = sf_mem_init, .fini = sf_mem_fini)
{
    size_t sz = 100;

    int limit = 200;
    int exp_mallocation = (MAX_SIZE - sizeof(sf_prologue) - sizeof(sf_epilogue)) / ADJUSTED_BLOCK_SIZE(sz);
    int exp_limit = limit - exp_mallocation;

    void * x;
    while ((x = sf_malloc(sz)) != NULL && limit--)
    {
        sf_block * bp = PAYLOAD_TO_BLOCK(x);
        size_t size = BLOCK_SIZE(bp);
        // Not all blocks will be the same size due to splitting restrictions.
        cr_assert(size == ADJUSTED_BLOCK_SIZE(sz) ||
                  size == (ADJUSTED_BLOCK_SIZE(sz) + (1 << ALIGN_SHIFT)),
                  "block has incorrect size (size=%lu, exp=%lu or %lu)",
                  size, ADJUSTED_BLOCK_SIZE(sz),
                  ADJUSTED_BLOCK_SIZE(sz) + (1 << ALIGN_SHIFT));
    }

fprintf(stderr, "limit: %d, exp_limit: %d, exp_mallocation: %d\n",
	limit, exp_limit, exp_mallocation);
    cr_assert_eq(limit, exp_limit, "Memory not exhausted when it should be");

    _assert_heap_is_valid();

    size_t exp_free_sz = MAX_SIZE - sizeof(sf_prologue) - sizeof(sf_epilogue)
                                  - ADJUSTED_BLOCK_SIZE(sz) * exp_mallocation;
    _assert_free_block_count(exp_free_sz, 1);

    _assert_errno_eq(ENOMEM);
}
