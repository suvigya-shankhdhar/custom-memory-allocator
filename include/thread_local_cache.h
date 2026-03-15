#ifndef THREAD_LOCAL_CACHE_H
#define THREAD_LOCAL_CACHE_H

#include <unistd.h>
#include <bits/mman-linux.h>
#include <pthread.h>

#define NUM_CLASSES 128             // 128 different number of classes, with an increment of 8-bytes (8, 16, 24,..., 1024)
#define HEADER_SIZE sizeof(Block); 
#define MAX_BLOCK_COUNT 128         // Maximum number of blocks a particular class can have.

// Header for each Block, stores a pointer to next Block.
struct Block {
    struct Block *next;             
}; 

// This structure is different for each thread.
// Consists an array of linked lists, with Block *head[i] pointing to first Block of i'th free list. 
// count[i] stores the number of free block available in i'th free list.  
struct ThreadCache {
    Block *head[NUM_CLASSES]; 
    int    count[NUM_CLASSES];
}; 

typedef struct Block Block; 
typedef struct ThreadCache ThreadCache; 

void initialise_tlc(); 
int get_size_idx(size_t); 
void* tlc_pop(int); 
void tlc_push(void*, int); 

#endif