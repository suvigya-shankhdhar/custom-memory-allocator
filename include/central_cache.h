#ifndef CENTRAL_CACHE_H
#define CENTRAL_CACHE_H

#include <unistd.h>
#include <bits/mman-linux.h>
#include <pthread.h>
#define NUM_CLASSES 128

struct CentralCache {
    Block *head[NUM_CLASSES]; 
    pthread_mutex_t locks[NUM_CLASSES];
}; 
typedef struct CentralCache CentralCache; 
CentralCache central_cache ; 

void grow_heap(int); 

#endif