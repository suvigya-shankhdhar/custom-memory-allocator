#ifndef CENTRAL_CACHE_H
#define CENTRAL_CACHE_H

#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include "internals.h"
#include <stdlib.h>

// Used to keep Central free lists for 
// all the size classes
struct CentralCache {
    Block *head[NUM_CLASSES]; 
    uint32_t count[NUM_CLASSES]; 
    pthread_mutex_t locks[NUM_CLASSES];
}; 
typedef struct CentralCache CentralCache; 


void init_central_cache(void);      // Initialize the central cache struct
size_t fetch_from_central_cache(uint8_t idx, Block **start, Block **end, size_t batch_size); // Extract a portion of central free list and add it to front of thread local list
void release_to_central_cache(uint8_t idx, Block *start, size_t batch_size); // Returns a portion of thread local list back to central free list
void grow_central_cache(uint8_t idx);   // If central free list is empty, grow it using master Arena

#endif