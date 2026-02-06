#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
int main(){
int s,c;
struct sockaddr_in sv;
char b[256];
s=socket(AF_INET,SOCK_STREAM,0);
sv.sin_family=AF_INET;
sv.sin_port=htons(8080);
sv.sin_addr.s_addr=INADDR_ANY;
bind(s,(struct sockaddr*)&sv,sizeof(sv));
listen(s,1);
printf("Waiting...\n");
c=accept(s,NULL,NULL);
while(1){
memset(b,0,256);
if(read(c,b,256)<=0)break;
printf("Client:%s\nYou:",b);
fgets(b,256,stdin);
write(c,b,strlen(b)+1);
}
close(c);
close(s);
return 0;
}
