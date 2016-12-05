#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int fd = -1;
    int n;
    struct sockaddr_in server;
    char sndbuff[1024], rcvbuff[1024];

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        printf("create socket error: %s\n", strerror(errno));
        exit(-1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(2500);
    inet_aton("127.0.0.1", &server.sin_addr);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    //connect(fd, (struct sockaddr *)&server, sizeof(server));

    while (fgets(sndbuff, 1024, stdin) != NULL) {
        sendto(fd, sndbuff, 1024, 0, (struct sockaddr *)&server, sizeof(server));
        n = recvfrom(fd, rcvbuff, 1024, 0, NULL, NULL);

        if (n < 0) {
            printf("recvfrom error: %s\n", strerror(errno));
            exit(-1);
        }
        rcvbuff[n] = '\0';
        fputs(rcvbuff, stdout);
    }
    close(fd);
    return 0;
}
