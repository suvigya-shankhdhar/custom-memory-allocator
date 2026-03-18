#include "include/central_cache.h"
#include "include/thread_local_cache.h"
#include "include/allocator.h"

void *heap_start = NULL; 
void *current_page = NULL; 
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
    if (current_page == NULL) current_page = heap_start; 
    void *new_page = current_page; 
    current_page = (char*)current_page + PAGE_SIZE; 
    int page_offset = ((char*)new_page - (char*)heap_start) / (PAGE_SIZE); 
    page_metadata[page_offset] = idx; 
    for(int offset = 0; offset + class_size <= PAGE_SIZE; offset += class_size) {
        Block *new_block = (Block*)((char*)new_page + offset); 
        new_block->next = central_cache.head[idx]; 
        central_cache.head[idx] = new_block; 
    }
}



