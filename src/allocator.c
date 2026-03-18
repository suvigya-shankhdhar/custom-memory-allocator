#include "include/allocator.h"
#include "include/thread_local_cache.h"
#include "include/central_cache.h"


void *large_alloc(size_t size) {

    void *new_block = mmap(NULL, size + LARGE_BLOCK_SIZE , PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
    LargeBlock* new_large_block = (LargeBlock *)new_block; 
    new_large_block->size = size + LARGE_BLOCK_SIZE; 
    return (void*)((char*)new_large_block + LARGE_BLOCK_SIZE); 
}


void *alloc(size_t size) {
    if (size == 0) {
        return NULL; 
    }
    else if (size > PAGE_SIZE) {
        return large_alloc(size); 

    }
    int idx = get_size_idx(size); 
    void *p = tlc_pop(idx); 
    if (p == NULL) {
        grow_heap(idx); 
    }
    p = tlc_pop(idx); 
    return p; 
}

void free(void *ptr){
    if (ptr == NULL) return ; 
    uintptr_t ptr_val = (uintptr_t)ptr; 
    uintptr_t arena_begin = (uintptr_t)heap_start; 
    uintptr_t arena_end = arena_begin + ARENA_SIZE; 
    if (ptr_val >= arena_begin &  ptr_val < arena_end)  {
        int page_offset = ((char*)ptr - (char*)heap_start) / PAGE_SIZE;    
        int idx = page_metadata[page_offset]; 
        tlc_push(ptr, idx);
    } 
    else {
        LargeBlock *header = (LargeBlock*)((char*)ptr - LARGE_BLOCK_SIZE); 
        size_t size = header->size; 
        munmap(header, size); 
    }     
}

