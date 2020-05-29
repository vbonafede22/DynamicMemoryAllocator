/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "sfmm.h"
#include "myheader.h"

int firstAlloc = 0;

void *sf_malloc(size_t size) {
    
    if(size == 0){
        return NULL;
    }

    //check if this is the first allocation or not
    if(firstAlloc == 0){
        firstAlloc = 1;         //set allocation to true
        
        //get first address from heap and sets it to prologue
        sf_prologue *prologue = (sf_prologue*)sf_mem_grow();

        //init prologue
        prologue -> header = (32 + 3);    //set alloced bit to 1
        prologue -> footer = prologue -> header ^ sf_magic();

        //get address to the end of the heap
        sf_epilogue *epilogue = (sf_epilogue*)sf_mem_end();
        //init epilogue
        epilogue = (void*)epilogue - 8; //subtract 8 bytes to get to the beginning of the epilogue
        epilogue -> header = 0x2; //set allocated bit to 1

        //get size of allocated and free blocks
        size_t empty_block = (PAGE_SZ - (sizeof(struct sf_prologue)) - (sizeof(struct sf_epilogue)));
        //printf("empty block: %ld\n", empty_block); // == 4048 | 4096 - 4048 = 48
        //printf("Size of epilogue: %ld\n", sizeof(struct sf_epilogue));
        
        //init header of first free block
        //printf("size of prologue: %ld\n", sizeof(struct sf_prologue)); // == 40
        sf_block *myHeader = (sf_block *)(sf_mem_start() + sizeof(struct sf_prologue) - sizeof(sf_footer));
        myHeader -> header = empty_block | 0x1;   
        
        //link next to first position in sf_free_list_heads
        // myHeader -> body.links.next = &sf_free_list_heads[1];
        // myHeader -> body.links.prev = &sf_free_list_heads[NUM_FREE_LISTS - 1];
        // sf_free_list_heads[NUM_FREE_LISTS - 1].body.links.next = myHeader;
        // sf_free_list_heads[1].body.links.prev = myHeader;

        for(int i = 0; i < NUM_FREE_LISTS; i++){    //i's were 1
            sf_free_list_heads[i].body.links.next = &sf_free_list_heads[i];
            sf_free_list_heads[i].body.links.prev = &sf_free_list_heads[i];
        }

        //set prev header
        myHeader -> prev_footer = prologue -> footer;  

        //init footer to current block 
        sf_footer *foot = (sf_footer *)(sf_mem_end() - sizeof(struct sf_epilogue) - sizeof(sf_footer));
        *foot = myHeader -> header ^ sf_magic();  

        size_t size = myHeader -> header & BLOCK_SIZE_MASK;
        add_to_free_list(size, myHeader);

        //sf_show_blocks();
        //sf_show_free_lists();
        //sf_show_heap();
    } 
    //finished init first round 
    
    size_t mySize = size + 16; //accounts for header size -> footer too??????

    //make sure block size is divisable by 16
    while(mySize % 16 != 0){
        mySize++;
    }

    //check if mysize is less then 32 if it is set it = to 32
    if(mySize < 32){
        mySize = 32;
    }

    //search free list that has the right amount of bytes
    int j = 0;
    int i = 0;
    int first_loop = 0;
    sf_block *free_block;
    //go threw free lists until it loops over
    while(true){        
        //printf("i: %d\n", i);
        sf_block *current_block = sf_free_list_heads[i].body.links.next;
        //printf("current_block: %ld\n", current_block -> header & BLOCK_SIZE_MASK);
        //if this is the correct size alloc at this block
        if((current_block -> header & BLOCK_SIZE_MASK) >= mySize || (current_block -> header & BLOCK_SIZE_MASK) != 0){
            //get head of sentiel 
            if((current_block -> header & BLOCK_SIZE_MASK) < mySize){       //i added this if else loop
                //printf("*********breaks\n");
                j = i;
                i = 0;
            } else {
                sf_block *my_header = current_block -> body.links.next;         
                //remove my_header -> body.links.next and use free_block
                free_block = my_header -> body.links.next;      //block to be alloced
                //more then one block in the list
                if(my_header -> body.links.next != my_header){
                    //shift next and prev
                    my_header -> body.links.next = free_block -> body.links.next;
                    free_block -> body.links.next -> body.links.prev = my_header;
                    break;
                }
            }
        }
        //if looped over so we need to call sf_mem_grow,        ******test this**********
        //printf("i: %d\n", i);
        //printf("first_loop: %d\n", first_loop);

        if(i == 0 && first_loop == 1){       //current_block -> body.links.next == sf_free_list_heads

            current_block = sf_free_list_heads[j].body.links.next;      // i added this,j was 0 
            //printf("sf_free_list_heads: %p\n", sf_free_list_heads);
            //printf("bln: %p\n", current_block -> body.links.next);
            //printf("********************gets in here\n");
            //sf_show_heap();
            void* page = sf_mem_grow();
            //if we run out of memory
            if(page == NULL){
                sf_errno = ENOMEM;
                return NULL;
            }

            //printf("gets in here\n");
            //colasece two pages together 
            //get address to the end of the heap
            sf_epilogue *epilogue = (sf_epilogue*)sf_mem_end();
            //create new epilogue
            epilogue = (void*)epilogue - 8; //subtract 8 bytes to get to the beginning of the epilogue
            epilogue -> header = 0x2; //set allocated bit to 1

            //get prev_alloc bit of prev epilougue
            sf_epilogue *prev_epilogue = page;
            prev_epilogue = (void*)prev_epilogue - 8;
            size_t prev_alloc_bit = (prev_epilogue -> header) & 0x00000001;
            //printf("------------------------------------\n");
            //printf("prev_epilogue: %ld\n", prev_epilogue -> header);
            if(prev_alloc_bit == 0){        //if prev block is free
                
                //combine headers and footers
                //get old footer
                sf_footer *free_foot = (void*)prev_epilogue - sizeof(sf_footer);
                size_t free_foot_size = (*free_foot ^ sf_magic()) & BLOCK_SIZE_MASK;    //size of old block
                //printf("foot_size %ld\n", free_foot_size);

                //printf("free foot size: %ld\n", free_foot_size);        //free foot size is negitive
                //get header of old free block
                
                sf_block *free_header = (void*)prev_epilogue - free_foot_size - 8;  
                //update old header 
                //printf("header location : %p\n", free_header);
                //printf("size = %ld\n", free_foot_size + PAGE_SZ);
                free_header -> header = (free_foot_size + PAGE_SZ) | 1;
                //printf("header after %ld\n", free_header -> header); 
                //printf("old size : %ld\n", free_header -> header);
                
                //update prev_footer
                //free_header -> prev_footer = 1008 + 3;       //hard coded 

                //create new footer
                sf_footer *foot = (void*)free_header + (free_header -> header & BLOCK_SIZE_MASK) + 8 - sizeof(sf_footer);
                *foot = free_header -> header ^ sf_magic();
                //printf("footer pointer : %p\n", foot );
                //sf_show_heap();   //correct up until here
                //add to correct position in free list, this might be wrong ******
                //sf_block *prev_free_list;
                //sf_block *current_free_list;
                sf_block *old_free_list;

                //free_header -> body.links.next = free_header;    //makes the next and prev point to the header, not right
                //free_header -> body.links.prev = free_header; 
                //sf_show_heap();
                
                //remove old free block 
                //old_free_list = check_size(free_foot_size);
                //printf("j val: %d\n", j);
                old_free_list = &sf_free_list_heads[j];         
                //printf("addr : %p \n", old_free_list);
                old_free_list -> body.links.next = old_free_list -> body.links.prev;
                
                sf_block *my_header = free_header;   //this was (void*)old_free_list -> header
                //printf("addr : %ld\n", my_header -> header);
                //sf_show_heap();

                sf_block *remove_block = my_header -> body.links.next;
                //printf("my_header -> body.links.next: %p\n", my_header -> body.links.next);
                while(remove_block != free_header){
                    //printf("remove_block: %p\n", remove_block);
                    //printf("free_header: %p\n", free_header);
                    //printf("next: %p\n", remove_block -> body.links.next);          //this is causing a problem 
                    remove_block = remove_block -> body.links.next;
                }
                //sf_show_heap();
                sf_block *prev_block = remove_block -> body.links.prev;
                sf_block *next_block = remove_block -> body.links.next;
                prev_block -> body.links.next = remove_block -> body.links.next;
                next_block -> body.links.prev = prev_block;
                //sf_show_heap();

                //check if theres already a list in that size 
                //sf_block *current_free_list;
                if(check_size(free_foot_size + PAGE_SZ) != NULL){
                    //list already exists, set it to current_free_List
                    //current_free_list = check_size(free_foot_size + PAGE_SZ);
                } else {
                    //printf("gets in here\n");
                    //prev_free_list = find_prev_node(free_foot_size + PAGE_SZ);
                    //printf("prev free list: %p\n", prev_free_list);
                    //current_free_list = add_to_free_list(free_foot_size + PAGE_SZ, prev_free_list);     //might not return the right thing 
                    add_to_free_list(free_foot_size + PAGE_SZ, free_header);     //might not return the right thing 

                }
                //printf("test%p\n", current_free_list);
                //sf_show_heap();
                //add free block to free node, might be wrong
                // sf_block *head = (void*)current_free_list -> header;
                // sf_block *next_head = head -> body.links.next;
                // head -> body.links.next = free_header;
                // next_head -> body.links.prev = free_header;
                // free_header -> body.links.next = next_head;
                // free_header -> body.links.prev = head;
            } else {
                //add a new page as a free block
                sf_block *new_header = (void*)prev_epilogue;
                
                //set header
                new_header -> header = PAGE_SZ + 3; //alloc and prev_alloc are set true

                //set prev_footer
                //new_header -> prev_footer = ;

                //set footer
                sf_footer *new_foot = (void*)new_header + (new_header -> header & BLOCK_SIZE_MASK) - sizeof(sf_footer);
                *new_foot = new_header -> header ^ sf_magic();

                //add to the list of free lists
                sf_block *prev_head;
                //sf_block *free_head;
                sf_footer *free_foot = (void*)prev_epilogue - sizeof(sf_footer);
                size_t free_foot_size = *free_foot & BLOCK_SIZE_MASK;
                //printf("make sure this is identical%ld\n", free_foot_size);
                if(check_size(new_header -> header & BLOCK_SIZE_MASK) != NULL){
                    //free_head = check_size(new_header -> header & BLOCK_SIZE_MASK);
                    //printf("free_head -> heeader : %ld\n", free_head -> header);
                } else {
                    prev_head = find_prev_node(free_foot_size + PAGE_SZ);
                    add_to_free_list(free_foot_size + PAGE_SZ, prev_head);      //might return the wront value
                }
                //add free block to free node, might be wrong
                // sf_block *head = (void*)free_head -> header;
                // sf_block *next_head = head -> body.links.next;
                // head -> body.links.next = new_header;
                // next_head -> body.links.prev = new_header;
                // new_header -> body.links.next = next_head;
                // new_header -> body.links.prev = head;
            } 
            //reset loop since we added into the free list
            //current_block = sf_free_list_heads[0].body.links.next;      //this is probably wrong
            i = 0;          //i added this                                                
        } else {
            current_block = current_block -> body.links.next;
        }
        if(i == 0){
            first_loop = 1;
        }
        i++;
    }
    //find if previous block in memory is allocated 
    //take the free block and allocate it
    //possibly spiltting into another free block if there is enough space 
    
    //printf("size of free_block: %ld\n", (free_block -> header & BLOCK_SIZE_MASK) - mySize); // == -32
    size_t split_int = (free_block -> header & BLOCK_SIZE_MASK) - mySize;
    //printf("split int: %ld\n", split_int);
    //no splitting 
    if(split_int >= 0 && split_int < 32){
        free_block -> header = size + 1;    //set header: mark prev allocated bit
        //sets the block following the allocated blocks prev alloc bit to 1
        set_prev_alloc_bit(free_block);

        //get payload
        void* payload = (void*)free_block + 16; //16 = prev_alloc and alloc 8+8
        //sf_show_blocks();
        return payload;
    } else {
        //splitting is required
        //size of split int is the size of the free block

        void *temp_block = free_block;

        //new block will become allocated + size of alloced block
        sf_block *new_block = temp_block + mySize;
        //printf("new_block: %p\n", new_block);

        //set header, mark prev alloced bit 
        new_block -> header = split_int + 1;    ///this is a free block
        //printf("new block: %ld\n", new_block -> header);

        //set prev_footer
        //new_block -> prev_footer = 35; //hard coded

        //create new footer
        temp_block = new_block;
        //sf_footer *foot = (void*)temp_block + (new_block -> header & BLOCK_SIZE_MASK) - sizeof(sf_footer);
        sf_footer *foot = (sf_footer *)(sf_mem_end() - sizeof(struct sf_epilogue) - sizeof(sf_footer));
        //printf("size of new %ld\n", new_block -> header & BLOCK_SIZE_MASK);
        *foot = new_block -> header ^ sf_magic();

        //sf_block *prev_block;
        //sf_block *current_block;

        if(check_size(split_int) != NULL){
            //if not null then it exists 
            //current_block = check_size(split_int);
            //printf("first current_block: %ld\n", current_block -> header);
        } else {
            //go through the free list lists and find where to put the block
            //printf("gets in herre\n");
            //prev_block = find_prev_node(split_int);
            //printf("******prev block: %p\n", prev_block);
            //current_block = add_to_free_list(split_int, prev_block);    //returning 0, new_block was prev_block
            add_to_free_list(split_int, new_block);
            //printf("current_block: %ld\n", current_block -> header);
        }

        //add free block to free node, this is wrong 
        // sf_block *head = (void*)new_block -> header;            //new block was current block 
        // //printf("head val: %ld\n", new_block -> header);
        // sf_block *next_head = head -> body.links.next;      //head -> body.links.next causing seg fault 
        // head -> body.links.next = new_block;
        // next_head -> body.links.prev = new_block;
        // new_block -> body.links.next = next_head;
        // new_block -> body.links.prev = head;

        //get prev alloc bit
        int prev_alloc_bit = free_block -> header & 0x00000001;

        //set header, free block is really the allocated block 
        free_block -> header = mySize + 2;
        free_block -> header = free_block -> header | prev_alloc_bit;

        //set prev footer, i added this 
        //free_block -> prev_footer = 35 ^ sf_magic();      //i added this, hard coded, not doing anything  

        //create new footer for free ist, i added this 
        sf_footer *free_footer = (void*)free_block + mySize;
        *free_footer = free_block -> header ^ sf_magic();

        void* payload = (void*)free_block + 16;     //16 accounts for header and prev_footer
        //sf_show_heap();
        //printf("payload %p\n", payload);
        return payload;
    }
    //sf_show_heap();
    return NULL;
}

void sf_free(void *pp) {
    pp = pp - 8;    //subtract header
    //printf("sf_free pointer: %p\n", pp);
    checkPointer(pp);

    sf_block *myHeader = pp;

    //get alloc_flag
    // size_t alloc_bit = myHeader -> header & 0x00000002;
    // alloc_bit = alloc_bit >> 1;
    // printf("check-alloc bit: %ld\n", alloc_bit);
    //get prev_alloc_flag
    size_t prev_bit = myHeader -> header & 0x00000001;
    //printf("check-prev_bit: %ld\n", prev_bit);
    size_t block_size = (myHeader -> header & BLOCK_SIZE_MASK);
    //printf("check-block size: %ld\n", block_size);
    //printf("my_header -> header%ld\n", myHeader -> header);
    sf_block *next_node = (void*)myHeader + block_size;
    size_t alloc_bit = next_node -> header & 0x00000002;
    alloc_bit = alloc_bit >> 1;


    if(prev_bit == 0 && alloc_bit == 0){
        //printf("gets in here 1\n");
        //get prev header
        sf_block *prev_header = (void *)(myHeader -> prev_footer) - block_size + 8;
        //get next header
        sf_block *next_header = (void *)myHeader + block_size;

        //coalesce current block with prev block 
        coalesce(myHeader, prev_header);

        //coalesce next block with prev block
        coalesce(next_header, prev_header);
    } else if(prev_bit == 0){   //coalesce previous 
        //printf("gets in here 2\n");
        //get prev header
        sf_block *prev_header = (void *)(myHeader -> prev_footer) - block_size + 8;
        coalesce(myHeader, prev_header);
    } else if(alloc_bit == 0){  //coalesce next 
        //printf("gets in here 3\n");
        //get next header
        sf_block *next_header = (void *)myHeader + block_size;
        coalesce(next_header, myHeader);
    } else {                    //dont coalesce
        //printf("gets in here 4\n");
        //set allocated to 0
        myHeader -> header = myHeader -> header & 0xfffffffd;

        //create footer
        sf_footer *free_footer = (void*)myHeader + block_size;
        //*free_footer = block_size | prev_bit;
        //printf("my header: %p\n", myHeader);
        //printf("my footer: %p\n", free_footer);
        *free_footer = myHeader -> header ^ sf_magic();

        //add new sized block to the free list if it doesnt already exist 
        sf_block *free_node = check_size(block_size);
        if(free_node == NULL){
            //sf_block *prev_node = find_prev_node(block_size);
            //add_to_free_list(block_size, prev_node);
            add_to_free_list(block_size, myHeader);
        }

        //add the header to that node 
        // sf_block *head_node = (void*)free_node -> header;
        // sf_block *next_node = head_node -> body.links.next;
        // head_node -> body.links.next = myHeader;
        // next_node -> body.links.prev = myHeader;
        // myHeader -> body.links.next = next_node;
        // myHeader -> body.links.prev = head_node;

        //set prev block prev bit to 0
        set_prev_alloc_bit_to_free(myHeader);
    }

    return;
}

void *sf_realloc(void *pp, size_t rsize) {
    //subtract header from pointer 
    pp = pp - 8;
    //printf("pointer: %p\n", pp);
    //printf("first rsize: %ld\n", rsize);

    //verify pointer is valid 
    checkPointer(pp);

    //if rsize == 0, free the block 
    if(rsize == 0){
        sf_free(pp + 8);
        return NULL;
    }

    //get block size
    sf_block *myHeader = pp;
    size_t block_size = myHeader -> header & BLOCK_SIZE_MASK;
    //printf("block_size: %ld\n", block_size);

    if((block_size - 8) < rsize){
        //realloc to a larger size

        void* alloc_block = sf_malloc(rsize);
        
        if(alloc_block == NULL){
            return NULL;
        }
        
        //memcpy(alloc_block, (pp + 8), rsize);
        //printf("realloc to larger\n");
        sf_free(pp + 8);
        
        return alloc_block;
    } else {
        //realloc to a bigger size

        size_t split_check = rsize + 8;
        
        while(split_check % 16 != 0){
            split_check++;
        }

        if(split_check < 32){
            split_check = 32;
        }

        if((block_size - split_check) < 32){
            //printf("return at splinter\n");
            return pp + 8;
        } else {
            myHeader -> header = split_check | 3;   //fix this 

            sf_block *free_block = (void*)myHeader + (myHeader -> header & BLOCK_SIZE_MASK);
            free_block -> header = (block_size - split_check) | 0x00000003;
            //printf("free_block -> header: %ld\n", free_block -> header);
            //printf("split rest into free block\n");
            sf_free(free_block + 8);
            
            return pp + 8;
        }
    }

    return NULL;
}

void checkPointer(void *pointer){
    void* end_of_pro = sf_mem_start(); 
    void* beg_of_epi = sf_mem_end();
    sf_block *myHeader = pointer;
    //printf("check pointer header: %ld\n", myHeader -> header);

    //check if pointer belongs to an allocated block 
    if(pointer < end_of_pro || pointer >= beg_of_epi){
        //printf("pointer belongs to an allocted block \n");
        abort();
    } 

    //check if pointer is null
    if(pointer == NULL){
        //printf("pointer is null | checkPointer\n");
        abort();
    }

    //check if allocated bit is 0, check if this is correct 
    //sf_block *myHeader = pointer;
    size_t alloc_bit = myHeader -> header & 0x00000002;
    //alloc_bit = alloc_bit >> 63;
    //printf("alloc bit: %ld\n", alloc_bit);
    if(alloc_bit == 0){
        //printf("alloc bit = 0\n");
        abort();
    }

    //check if prev_bit is 0 and if the alloc bit is 0
    size_t prev_bit = myHeader -> header << 63;
    prev_bit = prev_bit >> 63;
    //printf("prev bit: %ld\n", prev_bit);
    if(prev_bit == 0 && alloc_bit == 1){
        //printf("prev bit = 0\n");
        abort();
    }

    size_t block_size = myHeader -> header & BLOCK_SIZE_MASK;
    //printf("block size: %ld\n", block_size);
    //check if block size is less then 32
    if(block_size < 32){
        //printf("block size is less then 32\n");
        abort();
    }

    //check if footer XOR'd with sf_magic doesnt equal header   //******TODO 
    // if(myHeader -> header != ){
    //     printf("footer XOR'd with sf_magic doesnt equal header\n");
    //     abort();
    // }

}

void* coalesce(sf_block* current_header, sf_block* prev_header){
    //get block size of both block s
    size_t current_size = current_header -> header & BLOCK_SIZE_MASK;
    //printf("size of current block: %ld\n", current_size);
    size_t prev_size = prev_header -> header & BLOCK_SIZE_MASK;
    //printf("size of prev block: %ld\n", prev_size);

    //get size of new block
    size_t combined_size = current_size + prev_size;
    //printf("combined size: %ld\n", combined_size);

    //get blocks from the free list
    // sf_block *current_node = check_size(current_size);  
    // sf_block *prev_node = check_size(prev_size); 
    // printf("current_node -> header coalesce: %ld\n", current_node -> header);
    // printf("prev_node -> header coalesce: %ld\n", prev_node -> header);

    //get alloc bit of current node
    size_t current_alloc_bit = current_header -> header & 0x00000002;
    current_alloc_bit = current_alloc_bit >> 1;
    //printf("current alloc bit: %ld\n", current_alloc_bit);

    //get alloc bit of current node
    size_t prev_alloc_bit = prev_header -> header & 0x00000002;
    prev_alloc_bit = prev_alloc_bit >> 1;
    //printf("current alloc bit: %ld\n", prev_alloc_bit);

    if(current_header != NULL){
        if(current_header-> body.links.next != current_header || current_alloc_bit == 0){     
            //remove from free block aka re link the lists 
            //sf_block *head_node = (void*)current_node;
            sf_block *remove_block = current_header -> body.links.next;
            while(remove_block != current_header){
                //printf("remove_block: %p\n", remove_block);
                //printf("free_header: %p\n", free_header);
                //printf("next: %p\n", remove_block -> body.links.next);          
                //printf("****gets in here\n");
                remove_block = remove_block -> body.links.next;
            }

            sf_block *prev_block = remove_block -> body.links.prev;
            sf_block *next_block = remove_block -> body.links.next;
            prev_block -> body.links.next = remove_block -> body.links.next;
            next_block -> body.links.prev = prev_block;
        } 
    }

    // if(prev_header != NULL){
    //     if(prev_header -> body.links.next != prev_header || prev_alloc_bit == 0){        //current_node stuff might be wron
    //         //remove from free block aka relink the lists
    //         //sf_block *head_node = (void*)current_node;
    //         sf_block *remove_block = prev_header -> body.links.next;
    //         // while(remove_block != current_header){
    //         //     //printf("remove_block: %p\n", remove_block);
    //         //     //printf("free_header: %p\n", free_header);
    //         //     printf("next: %p\n", remove_block -> body.links.next);          //this is causing a problem 
    //         //     remove_block = remove_block -> body.links.next;
    //         // }

    //         sf_block *prev_block = remove_block -> body.links.prev;
    //         sf_block *next_block = remove_block -> body.links.next;
    //         prev_block -> body.links.next = remove_block -> body.links.next;
    //         next_block -> body.links.prev = prev_block;
    //     }
    // }

    //set combined headers
    prev_header -> header = combined_size | prev_alloc_bit; 
    //printf("prev -> header: %ld\n", prev_header -> header);
    //printf("*****prev_header: %p\n", prev_header);

    //create combined footer
    sf_footer *combined_footer = (void*)current_header + current_size;
    *combined_footer = prev_header -> header ^ sf_magic();
    //printf("*****combined_footer: %p\n", combined_footer);

    //add new block to free list 
    //sf_block *combined_node = check_size(combined_size); 
    //if(prev_header == NULL){
        //it doesnt already exist so we need to find where to put it
        size_t block_size = prev_header -> header & BLOCK_SIZE_MASK;
        //printf("block size: %ld\n", block_size);
        add_to_free_list(block_size, prev_header);
    //}

    //add the header, think this is wrong  
    // sf_block *head_node = (void*)combined_node -> header;
    // sf_block *next_node = head_node -> body.links.next;
    // head_node -> body.links.next = combined_node;
    // next_node -> body.links.prev = combined_node;
    // combined_node -> body.links.next = next_node;
    // combined_node -> body.links.prev = head_node;

    //set prev allocated to 0
    set_prev_alloc_bit_to_free(prev_header);

    return prev_header; //return combined_header;
}

void *add_to_free_list(size_t mySize, sf_block* myHeader){
    int index = 0;
    if(mySize == 32){
        //insert at 1st position in sf_free_list_heads
        index = 0;
    } else if (mySize > 32 && mySize <= 64){    //should this be mySize == 64???
        //insert at 2nd position in sf_free_list_heads + 1
        index = 1;
    } else if (mySize > 64 && mySize <= 128){
        //insert at 3rd position in sf_free_list_heads + 2
        index = 2;
    } else if (mySize > 128 && mySize <= 256){
        //insert at 4th position in sf_free_list_heads + 3
        index = 3;
    } else if (mySize > 256 && mySize <= 512){
        //insert at 5th position in sf_free_list_heads + 4
        index = 4;
    } else if (mySize > 512 && mySize <= 1024){
        //insert at 6th position in sf_free_list_heads + 5
        index = 5;
    } else if (mySize > 1024 && mySize <= 2048){
        //insert at 7th position in sf_free_list_heads + 6 
        index = 6;
    } else if (mySize > 2048 && mySize <= 4096){
        //insert at 8th position in sf_free_list_heads + 7
        index = 7;
    } else if (mySize > 4096){
        //insert at 9th position in sf_free_list_heads + 8
        //call sf_mem_grow() until the block is large enough
        index = 8;
    } else {
        //throw some sort of error
    }
    //printf("size: %ld\n", mySize);
    //printf("index: %d\n", index);

    myHeader -> body.links.next = &sf_free_list_heads[index];
    myHeader -> body.links.prev = &sf_free_list_heads[index];
    sf_free_list_heads[index].body.links.next = myHeader;
    sf_free_list_heads[index].body.links.prev = myHeader;

    return myHeader;        //want to return a pointer to the newly inserted node 
}

void *check_size(size_t my_size){
    int i = 0;
    //check if node of that size is already in the free list
    while(i != NUM_FREE_LISTS){     
        //printf("in check size\n");
        //sf_header *block_to_return = (void*)sf_free_list_heads[i].header;
        sf_block *return_block = &sf_free_list_heads[i];
        sf_block *current_block = sf_free_list_heads[i].body.links.next;
        size_t block_size = current_block -> header & BLOCK_SIZE_MASK;
        if(block_size == my_size){
            //return that block in which it was found
            return return_block;
        }
        //current_block = current_block -> body.links.next; 
        i++;
    }
    //return null if you cant find a block with that size
    //printf("returned null\n");
    return NULL;
}

void *find_prev_node(size_t my_size){
    //finds the node before the node to be inserted
    int i = 0;
    while(i != NUM_FREE_LISTS){     //change this to match the while loop in malloc
        //printf("in find prev node\n");
        sf_block *current_block = sf_free_list_heads[i].body.links.next;    
        if((current_block -> header ^ BLOCK_SIZE_MASK) >= my_size){
            //printf("current block size: %ld\n", current_block -> header ^ BLOCK_SIZE_MASK);
            //printf("current block returned : %p\n", current_block);
            return current_block;
        }
        current_block = current_block -> body.links.next; 
        i++;
    }
    return sf_free_list_heads;      //dont think this is right
}

void set_prev_alloc_bit(sf_block* my_header){
    //sets alloced of next block to 0
    sf_block *next_header = (void*)my_header + (my_header -> header & BLOCK_SIZE_MASK);
    int prev_bit = next_header -> header & 0x00000002;
    //printf("set prev alloc bit(prev_bit): %d\n", prev_bit);
    if(prev_bit == 0){
        next_header -> header = next_header -> header | 2;
        sf_footer *next_footer = (void*)next_header + (next_header -> header & BLOCK_SIZE_MASK) - sizeof(sf_footer);
        *next_footer = next_header -> header ^ sf_magic(); 
    }
}

void set_prev_alloc_bit_to_free(sf_block* my_header){
    //sets alloced of next block to 0
    size_t alloc_bit = my_header -> header & 0x00000002;
    sf_block *next_header = (void*)my_header + (my_header -> header & BLOCK_SIZE_MASK);
    //int prev_bit = next_header -> header & 0x00000001;
    //printf("set prev alloc bit(prev_bit): %d\n", prev_bit);

    if(alloc_bit == 0){
        next_header -> header = next_header -> header & 0xfffffffe;
        sf_footer *next_footer = (void*)next_header + (next_header -> header & BLOCK_SIZE_MASK) - sizeof(sf_footer);
        *next_footer = next_header -> header ^ sf_magic(); 
    }
}