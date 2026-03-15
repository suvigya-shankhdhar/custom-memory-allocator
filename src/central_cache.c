#include "include/central_cache.h"
#include "include/thread_local_cache.h"
#include "include/allocator.h"

void *heap_start = NULL; 
uint8_t page_metadata[MAX_PAGES]; 

// Initializes the central cache head[] with NULL ptr. 
void init_central_cache() {
    heap_start = mmap(NULL, ARENA_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    for(int i=0; i < NUM_CLASSES; i++) {
        central_cache.head[i] = NULL; 
        pthread_mutex_init(&central_cache.locks[i], NULL); 
    }
}

// Requests a new page from kernel using mmap(), 
// divides it into requested Class size and inerts them 
// into the corresponding free list.
void grow_heap(int idx) {
    
    size_t class_size = (idx+1)*8;
    size_t page_size = 4096; 
    static void *next_page = NULL; 
    if (next_page == NULL) next_page = heap_start; 
    void *new_page = next_page; 
    next_page = (char*)next_page + PAGE_SIZE; 
    (NULL, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    for(int offset = 0; offset + class_size <= page_size; offset += class_size) {
        Block *new_block = (Block*)((char*)new_page + offset); 
        new_block->next = central_cache.head[idx]; 
        central_cache.head[idx] = new_block; 
    }
}



