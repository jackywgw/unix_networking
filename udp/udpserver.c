#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int rc;
    int fd;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len;
    unsigned char recvbuf[1024];
    int n;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        printf("create socket error: %s\n",strerror(errno));
        exit(-1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(2500);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    len = sizeof(server);
    rc = bind(fd, (struct sockaddr*)&server, len);
    
    if (rc < 0) {
        printf("bind error: %s\n",strerror(errno));
        exit(-1);
    }
    

    for (;;) {
        len = sizeof(client);
        n = recvfrom(fd, recvbuf, 1024, 0, (struct sockaddr *)&client, &len);
        printf("recv client %s\n", inet_ntoa(client.sin_addr));  
        if (n < 0) {
            printf("recvfrom error: %s\n",strerror(errno));
            exit(-1);
        } else if (n == 0) {
            break;
        }
        
        sendto(fd, recvbuf, n, 0, (struct sockaddr *)&client, sizeof(client));
    }

    return 0;
}
