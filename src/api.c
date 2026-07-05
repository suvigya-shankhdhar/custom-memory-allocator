#include "../include/my_alloc.h"
#include "internals.h"
#include "thread_local_cache.h"
#include "central_cache.h"

static pthread_once_t init_once = PTHREAD_ONCE_INIT; 
extern __thread ThreadCache *tlc;

// If requested size > PAGE_SIZE, call large_allocation(),
// otherwise return Block just greater than requested size
// from thread local cache.
// Returns NULL, if failed
void *my_alloc(size_t size) { 
    if (size == 0) { return NULL; }
    pthread_once(&init_once, init_central_cache); 
    if (tlc == NULL) {
        init_thread_local_cache(); 
    }
    if (size > PAGE_SIZE) {
        return large_allocation(size); 
    }
    
    uint8_t idx = _index(size); 
    void *ptr = tlc_pop(idx); 
    if (ptr == NULL) {
        grow_thread_local_cache(idx); 
        ptr = tlc_pop(idx); 
    }
    return ptr;
}


void my_free(void *ptr) {
    if (ptr == NULL) {return; }
    if((uintptr_t*)ptr >= (uintptr_t*)heap_start && (uintptr_t*)ptr < (uintptr_t*)heap_end){
        uint8_t idx = index_from_ptr(ptr); 
        tlc_push(ptr, idx); 

    }
    else {
        free_large_allocation(ptr); 
    }
}

