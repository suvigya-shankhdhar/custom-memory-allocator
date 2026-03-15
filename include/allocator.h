#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <unistd.h>
#include <stdint.h>

#define ARENA_SIZE (1024 * 1024 * 64) // 64 MB Arena
#define MAX_PAGES (ARENA_SIZE / PAGE_SIZE)
#define PAGE_SIZE 4096

extern void *heap_start; 

extern uint8_t page_metadata[MAX_PAGES]; 

void init_allocator(); 
void *alloc(size_t size); 
void free(void *p); 

#endif

