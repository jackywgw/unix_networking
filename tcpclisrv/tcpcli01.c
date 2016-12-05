#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#define SERVER_PORT 9877
#define MAXBUFF 1024

int readline(int fd, char* buff, int len)
{
    int n;
    int i;
    char c;
    int count = 0;

    for (i = 0; i < len; i++) {
        n = read(fd, &c, 1);    
        if (n < 0) {
            printf("read error: %s\n",strerror(errno));
            return -1;
        }else if (n == 1) {
            buff[count++] = c;
            if (c == '\n') {
                break;
            }
        }else if(n == 0) {
            break;
        }
    }

    buff[count] = '\0';
    return count;
}

int main(int argc, char *argv[])
{
    int fd;
    int rc;
    int n;
    char buff[MAXBUFF];
    char recvbuff[MAXBUFF];
    struct sockaddr_in server;

    if (argc != 2) {
        printf("usage: a.out <x.x.x.x>\n");
        return -1;
    }
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("create socket error: %s!\n",strerror(errno));
        return -1;
    }

    server.sin_family = AF_INET;
    inet_aton(argv[1],&server.sin_addr);
    server.sin_port = htons(SERVER_PORT);
    
    rc = connect(fd, (struct sockaddr*)&server,sizeof(server));
    if (rc != 0) {
        printf("connect error: %s!\n",strerror(errno));
        return rc;
    }


    while (fgets(buff, MAXBUFF, stdin) != NULL) {
        write(fd, buff, strlen(buff));
        n = readline(fd, recvbuff, MAXBUFF); 
        if (n > 0) {
            fputs(recvbuff,stdout);
        }
        printf("n = %d\n",n);
    }

    close(fd);
    return 0;
}
