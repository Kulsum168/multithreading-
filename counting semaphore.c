#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
sem_t gate;
void* process_task(void* arg){
    int id=*(int*)arg;
    printf("User %d is standing in line...\n",id);
    sem_wait(&gate);
    printf("User %d is now using the resource.\n",id);
    sleep(rand()%2+1);
    printf("User %d finished and left.\n",id);
    sem_post(&gate);
    return NULL;
}
int main(){
    pthread_t users[6];
    int user_ids[6];
    sem_init(&gate,0,2);
    for(int i=0;i<6;i++){
        user_ids[i]=i+1;
        pthread_create(&users[i],NULL,process_task,&user_ids[i]);
    }
    for(int i=0;i<6;i++)
        pthread_join(users[i],NULL);
    sem_destroy(&gate);
    return 0;
}
