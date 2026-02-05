#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
int jobs = 0;
int live = 1;

void* start_worker(void* arg) {
    int id = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&mtx);
        while (jobs == 0 && live) {
            pthread_cond_wait(&cv, &mtx);
        }
        if (!live && jobs == 0) {
            pthread_mutex_unlock(&mtx);
            break;
        }
        jobs--;
        printf("Thread %d picked up a job\n", id);
        pthread_mutex_unlock(&mtx);
        usleep(100000);
    }
    printf("Thread %d done\n", id);
    return NULL;
}

int main() {
    pthread_t th[3];
    int val[3] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        pthread_create(&th[i], NULL, start_worker, &val[i]);
    }
    for (int i = 0; i < 6; i++) {
        pthread_mutex_lock(&mtx);
        jobs++;
        pthread_cond_signal(&cv);
        pthread_mutex_unlock(&mtx);
        usleep(200000);
    }
    pthread_mutex_lock(&mtx);
    live = 0;
    pthread_cond_broadcast(&cv);
    pthread_mutex_unlock(&mtx);
    for (int i = 0; i < 3; i++) {
        pthread_join(th[i], NULL);
    }
    return 0;
}
