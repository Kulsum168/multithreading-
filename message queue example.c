//sender 
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
int main(){
mqd_t mq;
char*msg="Hello";
mq=mq_open("/test",O_CREAT|O_WRONLY,0644,NULL);
mq_send(mq,msg,strlen(msg)+1,0);
printf("%s\n",msg);
mq_close(mq);
return 0;
}
// receiver 
#include <stdio.h>
#include <mqueue.h>
int main(){
mqd_t mq;
char buf[1024];
mq=mq_open("/test",O_RDONLY);
mq_receive(mq,buf,1024,NULL);
printf("%s\n",buf);
mq_close(mq);
mq_unlink("/test");
return 0;
}
