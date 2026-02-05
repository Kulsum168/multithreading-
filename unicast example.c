#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
int ready=0;
void* receiver(void* arg){
pthread_mutex_lock(&mtx);
while(!ready){
printf("waiting...\n");
pthread_cond_wait(&cond,&mtx);
}
printf("received!\n");
pthread_mutex_unlock(&mtx);
return NULL;
}
void* sender(void* arg){
sleep(1);
pthread_mutex_lock(&mtx);
ready=1;
printf("signaling...\n");
pthread_cond_signal(&cond);
pthread_mutex_unlock(&mtx);
return NULL;
}
int main(){
pthread_t t1,t2;
pthread_create(&t1,NULL,receiver,NULL);
pthread_create(&t2,NULL,sender,NULL);
pthread_join(t1,NULL);
pthread_join(t2,NULL);
return 0;
}
