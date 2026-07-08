#ifndef MY_ALLOC_H
#define MY_ALLOC_H
#include <unistd.h>
#include <stdint.h>

void *my_alloc(size_t size); 
void my_free(void *ptr); 
void *my_realloc(void *ptr, size_t size); 
void *my_calloc(size_t nmemb, size_t size); 
void  my_malloc_stats(void);

#endif