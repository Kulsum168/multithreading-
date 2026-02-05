#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
pthread_mutex_t m;
void* task(void* a) {
    int n = *(int*)a;
    pthread_mutex_lock(&m);
    printf("Access %d\n", n);
    sleep(1);
    printf("Done %d\n", n);
    pthread_mutex_unlock(&m);
    return NULL;
}
int main() {
    pthread_t t[3];
    int v[3] = {10, 20, 30};
    pthread_mutex_init(&m, NULL);
    for(int i=0; i<3; i++) {
        pthread_create(&t[i], NULL, task, &v[i]);
    }
    for(int i=0; i<3; i++) {
        pthread_join(t[i], NULL);
    }
    pthread_mutex_destroy(&m);
    return 0;
}
