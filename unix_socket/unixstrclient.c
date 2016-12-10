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
    int num;
    int nbytes;
    int i;
	int fd_tmp;
    int epoll_fd;
    char recvbuf[1024], sndbuff[1024];
    struct sockaddr_un server;
    struct epoll_event revents[MAX_EPOLL_EVENTS];

    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("create socket error: %s\n", strerror(errno));
        exit(-1);
    }

    bzero(&server, sizeof(server));

    server.sun_family = AF_LOCAL;
    strcpy(server.sun_path, "/home/jackywgw/unix_networking/unix_socket/tyx");

    rc = connect(fd, (struct sockaddr *)&server, SUN_LEN(&server));
    if (rc  < 0) {
        printf("connect error: %s\n", strerror(errno));
        exit(-1);
    }


    epoll_fd = epoll_create(MAX_EPOLL_EVENTS);
    if (epoll_fd < 0) {
        printf("create epoll error: %s\n", strerror(errno));
        exit(-1);
    }
    epoll_add_event(epoll_fd, fileno(stdin));
    epoll_add_event(epoll_fd, fd);

    for(;;) {
        num = epoll_wait(epoll_fd, revents, MAX_EPOLL_EVENTS, -1);

        if (num < 0) {
            printf("epoll wait error: %s\n", strerror(errno));
            break;
        }
        for (i = 0; i < num; i++) {
			fd_tmp = revents[i].data.fd;
			if (fd_tmp == fileno(stdin)) {
                /*please be careful: 
				 * recv only use for socket fd
				 * read can be used for every fd,
				 * so here we must use read, but not recv*/

				nbytes = read(fd_tmp, sndbuff, 1024);
                if (nbytes < 0) {
                    printf("read error: %s\n", strerror(errno));
                    exit(-1); // if read from stdin error, just exit the program
				} else if (nbytes == 0) {
					/*when ctrl+d for stdin, read will return 0*/
					//printf("nbytes ==0 ....\n");
                    shutdown(fd, SHUT_WR); /*here shutdown fd, but not fd_tmp*/
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd_tmp, NULL);
                } else {
					send(fd, sndbuff, nbytes, 0); 
                }

            } else if (fd_tmp == fd) {
                nbytes = recv(fd_tmp, recvbuf, 1024, 0);
                if (nbytes < 0) {
                    printf("recv error: %s\n", strerror(errno));
					if (errno == EINTR) {
						continue;
					} else {
                    	exit(-1); //if recv error, exit the program.
					}
				} else if (nbytes == 0) {
                    close(fd_tmp);
					exit(-1);
                    //epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd_tmp, NULL);
                } else {
					/* write can be used for every file description
					 * send only used for socket file description
					 * so here we must use write, not send*/
                    write(fileno(stdout), recvbuf, nbytes); 
                }

            }
        }
    }


    return 0;
}
