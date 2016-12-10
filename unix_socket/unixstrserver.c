#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/un.h>
#define MAX_EPOLL_EVENTS 100

void epoll_add_event(int epoll_fd, int fd)
{
    int rc = 0;
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    rc = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    if (rc < 0) {
        printf("epoll ctl error: %s\n", strerror(errno));
    }
}

int main(int argc, char *argv[])
{
    int rc = 0;
    int fd;
    int fd_tmp;
	int connfd;
    int num;
    int nbytes;
    int i;
    int epoll_fd;
    char recvbuf[1024];
    struct sockaddr_un server, client;
    socklen_t len;
    struct epoll_event revents[MAX_EPOLL_EVENTS];

    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("create socket error: %s\n", strerror(errno));
        exit(-1);
    }

    bzero(&server, sizeof(server));

    server.sun_family = AF_LOCAL;
    strcpy(server.sun_path, "/home/jackywgw/unix_networking/unix_socket/tyx");
    unlink("/home/jackywgw/unix_networking/unix_socket/tyx");

    rc = bind(fd, (struct sockaddr *)&server, SUN_LEN(&server));
    if (rc  < 0) {
        printf("bind error: %s\n", strerror(errno));
        exit(-1);
    }

    listen(fd, 50);

    epoll_fd = epoll_create(MAX_EPOLL_EVENTS);
    if (epoll_fd < 0) {
        printf("create epoll error: %s\n", strerror(errno));
        exit(-1);
    }

    epoll_add_event(epoll_fd, fd);

    for(;;) {
        num = epoll_wait(epoll_fd, revents, MAX_EPOLL_EVENTS, -1);

        if (num < 0) {
            printf("epoll wait error: %s\n", strerror(errno));
            break;
        }
        for (i = 0; i < num; i++) {
            if (revents[i].data.fd == fd) {
                len = sizeof(client);
                connfd = accept(fd, (struct sockaddr *)&client, &len);
                if (connfd < 0) {
                    printf("accept error: %s\n", strerror(errno));
                    break;
                }
                epoll_add_event(epoll_fd, connfd);
            } else {
				fd_tmp = revents[i].data.fd;
                nbytes = recv(fd_tmp, recvbuf, 1024, 0);
                if (nbytes < 0) {
                    printf("recv error: %s\n", strerror(errno));
                    close(fd_tmp);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd_tmp, NULL);
                } else if (nbytes == 0) {
                    close(fd_tmp);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd_tmp, NULL);
                } else {
                    send(fd_tmp, recvbuf, nbytes, 0); 
                }

            }
        }
    }


    return 0;
}
