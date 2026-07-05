#ifndef THREAD_LOCAL_CACHE_H
#define THREAD_LOCAL_CACHE_H

#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include "internals.h"

#define MAX_BLOCK_COUNT 128     // Maximum number of blocks a particular class can have.

// This structure is different for each thread.
// Consists an array of linked lists, with Block *head[i] pointing to first Block of i'th free list. 
// count[i] stores the number of free block available in i'th free list.  
struct ThreadCache {
    Block *head[NUM_CLASSES]; 
    int    count[NUM_CLASSES];
}; 
typedef struct ThreadCache ThreadCache; 

void init_thread_local_cache();     // Initialize ThreadCache struct
// int get_class_idx(size_t);      // 
void* tlc_pop(uint8_t idx);        // Pop and return the head of free list corresponding to idx
void tlc_push(void*, uint8_t);     // 
void grow_thread_local_cache(uint8_t idx);  
void release_from_thread_local_cache(uint8_t idx); 
#endif