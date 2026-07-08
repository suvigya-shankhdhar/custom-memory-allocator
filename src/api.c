#include <string.h>

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


void *my_calloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0){
        return NULL; 
    }
    size_t total_size = nmemb * size; 
    void *ptr = my_alloc(total_size);
    if (total_size > PAGE_SIZE){
        return memset(ptr, 0, total_size);
    }
    return ptr;
}


void *my_realloc(void *ptr, size_t size) {
    if (ptr == NULL) {
        return my_alloc(size); 
    }
    if (size == 0) {
        my_free(ptr); 
        return NULL; 
    }
    size_t old_size = 0; 
    if ((uintptr_t *)ptr >= (uintptr_t *)heap_start && (uintptr_t *)ptr < (uintptr_t *)heap_end) {
        old_size = get_class_size(index_from_ptr(ptr));
    }
    else {
        LargeBlock *large_block = (LargeBlock* )((char*) ptr - LARGE_BLOCK_SIZE); 
        old_size = large_block->size - LARGE_BLOCK_SIZE; 
    }

    if (size <= old_size) {
        return ptr; 
    }
     
    void *new_ptr = my_alloc(size); 
    if (new_ptr == NULL) {
        return NULL; 
    }
    memcpy(new_ptr, ptr, old_size);  
    my_free(ptr); 
    return new_ptr; 
}