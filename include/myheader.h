#include "sfmm.h"

void checkPointer(void *pointer);
void* coalesce(sf_block* current_header, sf_block* prev_header);
void *add_to_free_list(size_t mySize, sf_block* myHeader);
void *check_size(size_t my_size);
void *find_prev_node(size_t my_size);
void set_prev_alloc_bit(sf_block* my_header);
void set_prev_alloc_bit_to_free(sf_block* my_header);