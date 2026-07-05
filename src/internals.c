#include "internals.h"

// Returns a block of size in multiple of PAGE_SIZE, 
// this is just greater than requested size 
void *large_allocation(size_t size) {
    size_t aligned_size = (size + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
    void* raw_ptr = mmap(NULL, aligned_size + LARGE_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (raw_ptr == MAP_FAILED) { return NULL; } 
    LargeBlock* new_large_block = (LargeBlock*) raw_ptr;
    new_large_block->size = aligned_size + LARGE_BLOCK_SIZE; 
    return (char*)new_large_block + LARGE_BLOCK_SIZE;  
}


void free_large_allocation(void *ptr) {
    if (ptr == NULL) {return; }
    LargeBlock* large_block = (LargeBlock*)((char*)ptr - LARGE_BLOCK_SIZE); 
    size_t size = large_block->size; 
    munmap(large_block, size);
}

// Returns index of size class having size just greater
// than requested size
uint8_t _index(size_t size) {

    // Tier 1: 8B to 128B (8-byte intervals) (0, 1, 2,...., 15) (16 Classes)
    if (size <= 128) {
        return ((size + 7)/8) - 1; 
    }

    // Tier 2: 129B to 512B (32-byte intervals) (16, 17,...., 27) (12 Classes)
    if (size <= 512) {
        return 16 + ((size -128 + 31)/32) - 1; 
    }

    // Tier 3: 513B to 1024B (128-byte intervals) (28, 29,..., 31) (4 Classes)
    if (size <= 1024) {
        return 28 + ((size - 512 + 127)/128) - 1; 
    }       

    // Tier 4: 1025B to 4096B ( 512-byte inteval) (32, 33,..., 37) (6 Classes)
    return 32 + (size - 1024 + 511)/512 - 1; 
}

uint8_t index_from_ptr(void *ptr) {
    size_t page_offset = ((char*)ptr - (char*)heap_start)/PAGE_SIZE; 
    return page_metadata[page_offset];
}



size_t get_class_size(uint8_t idx) {
    if (idx < 16) {return (idx+1)*8; }
    if (idx < 28) {return 128 + (idx-15) * 32; } 
    if (idx < 32) {return 512 + (idx-27)*128; }
    return 1024 + (idx - 31 )* 512; 
}