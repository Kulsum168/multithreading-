#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 5
int queue[MAX];
int head = 0, tail = 0, total = 0;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t has_space = PTHREAD_COND_INITIALIZER;
pthread_cond_t has_data = PTHREAD_COND_INITIALIZER;

void* sender(void* p) {
    for (int i = 1; i <= 10; i++) {
        pthread_mutex_lock(&mtx);
        while (total == MAX) {
            pthread_cond_wait(&has_space, &mtx);
        }
        queue[head] = i;
        printf("[+] Added: %d\n", queue[head]);
        head = (head + 1) % MAX;
        total++;
        pthread_cond_signal(&has_data);
        pthread_mutex_unlock(&mtx);
        usleep(100000);
    }
    return NULL;
}

void* receiver(void* p) {
    for (int i = 1; i <= 10; i++) {
        pthread_mutex_lock(&mtx);
        while (total == 0) {
            pthread_cond_wait(&has_data, &mtx);
        }
        int val = queue[tail];
        printf("[-] Removed: %d\n", val);
        tail = (tail + 1) % MAX;
        total--;
        pthread_cond_signal(&has_space);
        pthread_mutex_unlock(&mtx);
        usleep(300000);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, sender, NULL);
    pthread_create(&t2, NULL, receiver, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
