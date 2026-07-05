#ifndef INTERNALS_H
#define INTERNALS_H

#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
// #include "central_cache.h"
// #include "thread_local_cache.h"

#define ARENA_SIZE (1024ULL * 1024 * 1024 * 1)  // Maximum Arena Size is 1GB
#define PAGE_SIZE 4096  // Page size is 4KB
#define MAX_PAGES (ARENA_SIZE/PAGE_SIZE)    // Maxium Number of pages is 250,000
#define NUM_CLASSES 38  // Number of different size classes


// Used for allocations > PAGE_SIZE (4KB)
struct LargeBlock {
    size_t size; 
}; 
typedef struct LargeBlock LargeBlock; 

// Used for both thread local lists and central lists.
struct Block {
    struct Block *next; 
}; 
typedef struct Block Block; 

#define LARGE_BLOCK_SIZE sizeof(LargeBlock) 
#define BLOCK_SIZE sizeof(Block)

extern void *heap_start;    // Points to head of master Arena
extern void *heap_end;      // Points to end of master Arena
extern uint8_t page_metadata[MAX_PAGES];    // Keeps track of which size class each page belongs to

void *large_allocation(size_t size);    // Handles allocations > PAGE_SIZE (4KB)
void free_large_allocation(void *ptr);      // Frees allocations > PAGE_SIZE (4KB)

uint8_t _index(size_t size);     // Returns index of size class having size just > requested size
uint8_t index_from_ptr(void *ptr);      // Returns index of size class from give pointer
size_t get_class_size(uint8_t idx);     // Returns size of size class from index

#endif