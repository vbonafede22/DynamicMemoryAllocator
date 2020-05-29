#ifndef SFMM_TEST_H
#define SFMM_TEST_H

#include <criterion/criterion.h>
#include "sfmm.h"
#include <signal.h>
#include <stdio.h>
#include <errno.h>

// ====================================================================================================
// updated macros

#define MAX_SIZE                  (4 * PAGE_SZ)

#define ALIGN_SHIFT               4U
#define ALIGN(v)                  ((((v) + (1U << ALIGN_SHIFT) - 1U) >> ALIGN_SHIFT) << ALIGN_SHIFT)
#define MIN_BLOCK_SIZE            ((size_t) (ALIGN(sizeof(sf_block))))

#define max(x, y)                 (((x) > (y)) ? (x) : (y))

#define ADJUSTED_BLOCK_SIZE(sz)   (max(ALIGN(sz + sizeof(sf_header) + sizeof(sf_footer)), MIN_BLOCK_SIZE))

#define ALIGNMENT_MASK            ((1U << ALIGN_SHIFT) - 1U)

#define ALIGNED_SIZE(size)        (((size) & ALIGNMENT_MASK) == 0)
#define ALIGNED_BLOCK(bp)         ((((uint64_t) bp) & (ALIGNMENT_MASK)) == 0)

#define BLOCK_SIZE(bp)            ((bp)->header & BLOCK_SIZE_MASK)

#define PAYLOAD_TO_BLOCK(pp)                                                                         \
    ((sf_block *) ((char *) (pp) - (char *) &(((sf_block *) 0x0)->body.payload)))

#define NEXT_BLOCK(bp)            ((sf_block *) ((char *) (bp) + BLOCK_SIZE(bp)))
#define FOOTER(bp)                (NEXT_BLOCK(bp)->prev_footer ^ sf_magic())

#define PREV_FOOTER(bp)           (bp->prev_footer ^ sf_magic())
#define PREV_BLOCK(bp)            ((sf_block *) ((char *) (bp) - (PREV_FOOTER(bp) & BLOCK_SIZE_MASK)))

/*
 * The prologue consists of an allocated block of minimum size starting
 * at whatever offset from the beginning of the heap is necessary to align
 * the payload area.
 */
#define PROLOGUE                  ((sf_prologue *)(sf_mem_start()))

/*
 * The epilogue consists of an allocated header at the end of the heap.
 */
#define EPILOGUE                  ((sf_epilogue *)(sf_mem_end() - sizeof(sf_epilogue)))
#define BLOCK_IS_EPILOGUE(bp)     ((char *)&(bp)->header != (char *)EPILOGUE)

/* Macros to maintain allocation status bits. */
#define ALLOCATED(bp)             (((bp)->header & THIS_BLOCK_ALLOCATED) != 0)
#define PREV_ALLOCATED(bp)        (((bp)->header & PREV_BLOCK_ALLOCATED) != 0)

#define VALID_BLOCK_SIZE(bp)                                                        \
   (BLOCK_SIZE(bp) >= MIN_BLOCK_SIZE &&                                             \
    BLOCK_SIZE(bp) <= 100 * PAGE_SZ &&                                              \
    ALIGNED_SIZE(BLOCK_SIZE(bp)))

#define VALID_FOOTER(bp)          (FOOTER(bp) == (bp)->header)

#define VALID_NEXT_BLOCK_PREV_ALLOC(bp)                                             \
    (PREV_ALLOCATED(NEXT_BLOCK(bp)) == ALLOCATED(bp))

#define LOOP_LIMIT                1000

// ====================================================================================================


void _assert_free_list_is_empty(void);

void _assert_prologue_is_valid(void);

void _assert_epilogue_is_valid(void);

void _assert_block_is_valid(sf_block * hp);

void _assert_heap_is_valid(void);

void _assert_block_info(sf_block * hp, int alloc, size_t b_size);

void _assert_nonnull_payload_pointer(void * pp);

void _assert_null_payload_pointer(void * pp);

void _assert_free_block_count(size_t size, int count);

void _assert_errno_eq(int n);

#endif
