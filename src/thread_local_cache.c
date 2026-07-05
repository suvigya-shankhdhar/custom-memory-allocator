#include "internals.h"
#include "thread_local_cache.h"
#include "central_cache.h"
#include <stdint.h>

// Defines a thread specific pointer to ThreadCache struct. 
__thread ThreadCache *tlc = NULL; 

// Allocates memory for tlc struct using mmap and 
// initialises all Block *head[] to NULL and count[] to 0.
void init_thread_local_cache() {
    tlc = (ThreadCache*)mmap(NULL, sizeof(ThreadCache), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
    for(int i = 0; i < NUM_CLASSES; i++){
        tlc->head[i] = NULL; 
        tlc->count[i] = 0; 
    }
}

// Returns a pointer to the first Block of the requested list (class),
// returns NULL if the list is empty. 
void *tlc_pop(uint8_t idx) {
    if (tlc->head[idx] == NULL) return NULL; 
    Block *temp = tlc->head[idx]; 
    tlc->head[idx] = temp->next; 
    (tlc->count[idx])--; 
    return (void*)temp; 
}

// Adds a free Block to the begining of free list, 
// if the number of free Blocks exceed MAX_BLOCK_COUNT (defined in thread_local_cache.c), half
// of the Blocks are returned to central free list.
void tlc_push(void* p, uint8_t idx) {
    Block *new_block = (Block*)p; 
    new_block->next = tlc->head[idx]; 
    tlc->head[idx] = new_block; 
    (tlc->count[idx])++; 
    if (tlc->count[idx] > MAX_BLOCK_COUNT) {
        release_from_thread_local_cache(idx); 
    }    
}


// Flush half of a thread local free list to the
// central free list, when a particular free list exceeds
// MAX_BLOCK_COUNT (defined in thread_local_cache.c). 
void release_from_thread_local_cache(uint8_t idx) {
    
    Block *head = tlc->head[idx]; 
    Block *temp = head;
    for (int i = 0; i < MAX_BLOCK_COUNT/2; i++) {
        temp = temp->next;
    }
    tlc->head[idx] = temp->next; 
    temp->next = NULL; 
    release_to_central_cache(idx, head, MAX_BLOCK_COUNT/2);
}

//
void grow_thread_local_cache(uint8_t idx) {
    
    Block *batch_start = NULL; 
    Block *batch_end = NULL; 
    size_t target_batch = MAX_BLOCK_COUNT / 2; 
    size_t allocated = fetch_from_central_cache(idx, &batch_start, &batch_end,target_batch); 
    
    if (allocated == 0) return;
    batch_end->next = tlc->head[idx];
    tlc->head[idx] = batch_start;
    tlc->count[idx] += allocated; 
}