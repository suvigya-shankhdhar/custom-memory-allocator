#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "include/my_alloc.h"

void *thread_work(void *arg) {
    int id = *(int*)arg;
    printf("Thread %d starting...\n", id); 

    for (int i = 0; i < 1000; i++) {

        char *small_str = (char*) my_alloc(32); 
        strcpy(small_str, "Hello Thread Cache!"); 
        my_free(small_str); 

        int *large_arr = (int *)my_alloc(8921); 
        large_arr[0] = id; 
        my_free(large_arr); 
    }

    printf("Thread %d finished successfully.\n", id); 
    return NULL; 
}

int main() {
    pthread_t threads[4]; 
    int thread_ids[4] = {1, 2, 3, 4};
        for (int i = 0; i < 4; i++) {
            pthread_create(&threads[i], NULL, thread_work, &thread_ids[i]); 
        }
        for (int i = 0; i < 4; i++) {
            pthread_join(threads[i], NULL); 
        }
    printf("All allocations and frees completed successfully\n");
    return 0; 
}