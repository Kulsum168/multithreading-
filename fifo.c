#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>

int fd_in,fd_out;

void* receive_logs(void* ptr){
    char incoming[512];
    while(read(fd_in,incoming,512)>0){
        printf("\rRemote: %s\nLocal: ",incoming);
        fflush(stdout);
    }
    return NULL;
}

int main(int argc,char** argv){
    if(argc<2) exit(1);
    mkfifo("pipe1",0666);
    mkfifo("pipe2",0666);
    if(argv[1][0]=='1'){
        fd_out=open("pipe1",O_WRONLY);
        fd_in=open("pipe2",O_RDONLY);
    }else{
        fd_in=open("pipe1",O_RDONLY);
        fd_out=open("pipe2",O_WRONLY);
    }
    pthread_t net_thread;
    pthread_create(&net_thread,NULL,receive_logs,NULL);
    char keyboard_buf[512];
    while(1){
        printf("Local: ");
        fflush(stdout);
        if(!fgets(keyboard_buf,512,stdin)) break;
        keyboard_buf[strcspn(keyboard_buf,"\n")]=0;
        write(fd_out,keyboard_buf,strlen(keyboard_buf)+1);
    }
    return 0;
}
