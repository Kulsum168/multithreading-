#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    int s;
    struct sockaddr_in sv;
    s = socket(AF_INET, SOCK_STREAM, 0);
    sv.sin_family = AF_INET;
    sv.sin_port = htons(8080);
    sv.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(s, (struct sockaddr*)&sv, sizeof(sv));
    char b[256];
    while(1) {
        printf("You: ");
        fgets(b, sizeof(b), stdin);
        write(s, b, strlen(b)+1);
        read(s, b, sizeof(b));
        printf("Server: %s\n", b);
    }
    return 0;
}
