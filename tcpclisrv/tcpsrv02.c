#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
/*add wait for child process to deal zombie process*/
#define SERVER_PORT 9877
#define MAXBUFF 1024

void sig_handle(int sigo)
{
    pid_t pid;
    int status;

    pid = wait(&status);

    printf("child %d terminated\n", pid);
    return;
}

void strecho(int connfd)
{
    char buff[MAXBUFF];
    int n;
    if(connfd < 0) {
        printf("connfd error!\n");
        return;
    }
    
again:
    while((n = read(connfd,buff,MAXBUFF)) > 0)
        write(connfd,buff,n);

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        printf("read error!\n");
}

int main(int argc, char *argv[])
{
    int listenfd,connfd;
    socklen_t len;
    struct sockaddr_in client_addr, server_addr;


    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        printf("create socket error!\n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("bind error!\n");
        return -1;
    }
    
    if (listen(listenfd, 20) < 0) {
        printf("listen error!\n");
        return -1;
    }
    signal(SIGCHLD, sig_handle);

    for(;;) {
        len = sizeof(client_addr);

        connfd = accept(listenfd, (struct sockaddr *)&client_addr, &len);
        if (connfd < 0) {
            printf("accept error :%s\n",strerror(errno));
            return -1;
        }
        
        if (fork() == 0) {
            close(listenfd);
            strecho(connfd);
            close(connfd);
            exit(0);
        }
        close(connfd);
    }

    return 0;
}
