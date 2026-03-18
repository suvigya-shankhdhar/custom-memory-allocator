#include "include/thread_local_cache.h"
#include "include/central_cache.h"

extern CentralCache central_cache; 

// Defines a thread specific pointer to ThreadCache struct. 
static __thread ThreadCache *tlc = NULL; 

// Allocates memory for tlc struct using mmap and 
// initialises all Block *head[] to NULL and count[] to 0.
void initialise_tlc() {
    tlc = (ThreadCache*)mmap(NULL, sizeof(ThreadCache), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
    for(int i = 0; i < NUM_CLASSES; i++){
        tlc->head[i] = NULL; 
        tlc->count[i] = 0; 
    }
}

// Returns the index of class which has a size greater than or equal to required size.
int get_class_idx(size_t size) {
    if (size == 0 ) return NULL; 
    for(int i = 0; i < NUM_CLASSES; i++) {
        if (size < (i+1)*8) {
            return i; 
        }
    }
}

// Returns a pointer to the first Block of the requested list,
// returns NULL if the list is empty. 
void *tlc_pop(int idx) {
    if (tlc->head[idx] == NULL) return NULL; 
    Block *temp = tlc->head[idx]; 
    tlc->head[idx] = temp->next; 
    tlc->count[idx]--; 
    return (void*)temp; 
}

// Adds a free Block to the begining of free list, 
// if the number of free Blocks exceed MAX_BLOCK_COUNT, half
// of the Blocks are returned to central free list.
void tlc_push(void* p, int idx) {
    Block *new_block = (Block*)p; 
    new_block->next = tlc->head[idx]; 
    tlc->head[idx] = new_block; 
    tlc->count[idx]++; 
    if (tlc->head[idx] > MAX_BLOCK_COUNT) {
        tlc_flush(idx); 
    }    
}

void tlc_flush(int idx) {
    pthread_mutex_lock(&(central_cache.locks[idx])); 
    int blocks_to_move = tlc->count[idx]; 

    for(int i = 0; i < blocks_to_move; i++) {
        Block *move_block = tlc->head[idx]; 
        tlc->head[idx] = move_block->next; 

        move_block->next = central_cache.head[idx]; 
        central_cache.head[idx] = move_block; 

        tlc->count[idx]--; 

    }
    pthread_mutex_unlock(&(central_cache.locks[idx])); 
}
