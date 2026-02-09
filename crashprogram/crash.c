#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int *ptr;

void* task(void* arg) {
    // Crash: Accessing memory that might be NULL or freed
    printf("Value is: %d\n", *ptr); 
    return NULL;
}

int main() {
    pthread_t t1;
    ptr = malloc(sizeof(int));
    *ptr = 100;

    pthread_create(&t1, NULL, task, NULL);
    
    // Immediate free: The thread and main race to this pointer
    free(ptr);
    ptr = NULL; 

    pthread_join(t1, NULL);
    return 0;
}
