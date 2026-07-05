#include "central_cache.h"
#include "internals.h"
#include <stdio.h>

// #include "include/thread_local_cache.h"
// #include "include/allocator.h"

CentralCache central_cache; 
uint8_t page_metadata[MAX_PAGES];

void *heap_start = NULL;    // Points to the start of master arena 
void *current_page = NULL;  // Points to the first page that is free
void *heap_end = NULL;      // Points to end of master arena

// Initializes the central cache struct 
void init_central_cache(void) {
    for (int i=0; i < NUM_CLASSES; i++ ){
        central_cache.head[i] = NULL; 
        central_cache.count[i] = 0; 
        pthread_mutex_init(&central_cache.locks[i], NULL); 
    }

    heap_start = mmap(NULL, ARENA_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
    if (heap_start == MAP_FAILED) {
        perror("Failed to allocate master arena"); 
        exit(1);
    }
    current_page = heap_start; 
    heap_end = (char*)heap_start + ARENA_SIZE; 
}

// Extract a portion of central free list, add it to front of thread local list
size_t fetch_from_central_cache(uint8_t idx, Block **start, Block **end, size_t batch_size) {
    pthread_mutex_lock(&central_cache.locks[idx]); 

    // If central cache is empty for this size, expand it from master Arena
    if(central_cache.head[idx] == NULL) {
        grow_central_cache(idx); 
        if (central_cache.head[idx] == NULL) { // If master Arena is full 
            pthread_mutex_unlock(&central_cache.locks[idx]); 
            return 0; 
        }
    }

    Block *curr = central_cache.head[idx]; 
    *start = curr; 

    size_t allocated = 0; 
    Block *prev = NULL; 
    while(curr != NULL && allocated < batch_size) {
        prev = curr;
        curr = curr->next; 
        allocated++; 
    }

    // Detach the batch from central cache list
    central_cache.head[idx] = curr; 
    central_cache.count[idx] -= allocated; 

    // Seal the end of the batch chain handed back to the thread
    if(prev != NULL) {
        prev->next = NULL; 
        *end = prev;
    }

    pthread_mutex_unlock(&central_cache.locks[idx]); 
    return allocated; 
}

//
void release_to_central_cache(uint8_t idx, Block *start, size_t batch_size) {
    pthread_mutex_lock(&central_cache.locks[idx]); 

    Block *curr = start; 
    while(curr->next != NULL) {
        curr = curr->next; 
    }

    curr->next = central_cache.head[idx];
    central_cache.head[idx] = start; 
    central_cache.count[idx] += batch_size; 

    pthread_mutex_unlock(&central_cache.locks[idx]); 
}

// Takes a new page from master arena, 
// divides it into Blocks of requested Class and inerts them 
// into corresponding central free list.
void grow_central_cache(uint8_t idx) {
    
    size_t class_size = get_class_size(idx); 
    void *new_page = current_page; 
    current_page = (char*)current_page + PAGE_SIZE; 
    size_t page_offset = ((char*)new_page - (char*)heap_start) / (PAGE_SIZE); 
    page_metadata[page_offset] = idx; 
    for(int offset = 0; offset + class_size <= PAGE_SIZE; offset += class_size) {
        Block *new_block = (Block*)((char*)new_page + offset); 
        new_block->next = central_cache.head[idx]; 
        central_cache.head[idx] = new_block; 
        central_cache.count[idx]++; 
    }
}



