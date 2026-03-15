#include "include/allocator.h"
#include "include/thread_local_cache.h"
#include "include/central_cache.h"





void *alloc(size_t size) {
    if (size == 0) {
        return NULL; 
    }
    else if (size > PAGE_SIZE) {
        return 
    }
    int idx = get_size_idx(size); 
    void *p = tlc_pop(idx); 
    if (p == NULL) {
        grow_heap(idx); 
    }
    p = get_size_idx(size); 
    return p; 
}

