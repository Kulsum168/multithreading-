#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define PORT 8080
#define MAXLINE 1024

int max(int x,int y){return (x>y)?x:y;}

int main(){
    int lfd,ufd,cfd,nready,maxfd;
    char buf[MAXLINE];
    fd_set rset;
    struct sockaddr_in saddr,caddr;
    socklen_t len;

    lfd=socket(AF_INET,SOCK_STREAM,0);
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_addr.s_addr=htonl(INADDR_ANY);
    saddr.sin_port=htons(PORT);

    int opt=1;
    setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    bind(lfd,(struct sockaddr*)&saddr,sizeof(saddr));
    listen(lfd,5);

    ufd=socket(AF_INET,SOCK_DGRAM,0);
    bind(ufd,(struct sockaddr*)&saddr,sizeof(saddr));

    maxfd=max(lfd,ufd)+1;
    printf("Server running on %d...\n",PORT);

    for(;;){
        FD_ZERO(&rset);
        FD_SET(lfd,&rset);
        FD_SET(ufd,&rset);

        nready=select(maxfd,&rset,NULL,NULL,NULL);

        if(FD_ISSET(lfd,&rset)){
            len=sizeof(caddr);
            cfd=accept(lfd,(struct sockaddr*)&caddr,&len);
            memset(buf,0,MAXLINE);
            read(cfd,buf,sizeof(buf));
            printf("TCP: %s\n",buf);
            write(cfd,"TCP_ACK",7);
            close(cfd);
        }

        if(FD_ISSET(ufd,&rset)){
            len=sizeof(caddr);
            memset(buf,0,MAXLINE);
            int n=recvfrom(ufd,buf,MAXLINE,0,(struct sockaddr*)&caddr,&len);
            printf("UDP: %s\n",buf);
            sendto(ufd,"UDP_ACK",7,0,(struct sockaddr*)&caddr,len);
        }
    }
    return 0;
}
//echo "Hello TCP" | nc -q 1 localhost 8080
//echo "Hello UDP" | nc -u -w 1 localhost 8080

