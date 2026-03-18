#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <unistd.h>
#include <stdint.h>

#define ARENA_SIZE (1024 * 1024 * 64) // 64 MB Arena
#define MAX_PAGES (ARENA_SIZE / PAGE_SIZE)
#define PAGE_SIZE 4096
#define LARGE_BLOCK_SIZE sizeof(LargeBlock)

extern void *heap_start; 
extern void *current_page; 

extern uint8_t page_metadata[MAX_PAGES]; 

struct LargeBlock { 
    size_t size; 
}; 
typedef LargeBlock LargeBlock; 

void init_allocator(); 
void *alloc(size_t); 
void free(void *); 

#endif

