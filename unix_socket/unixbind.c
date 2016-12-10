#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>
/*
 *struct sockaddr_un {
 *  sa_family_t sun_family;  //AF_LOCAL
 *  char        sun_path[104];
 *};
 * */
//#define SUN_LEN(ptr) ((size_t)(((struct sockaddr_un*)0)->sun_path) + strlen((ptr)->sun_path))
struct tyx_s {
    int a;
    short b;
    char c;
};
int main(int argc, char *argv[])
{
    int rc = 0;
    int fd;
    socklen_t len;
    struct sockaddr_un addr1, addr2;

    if (argc != 2) {
        printf("usage: a.out <path_name>\n");
        exit(-1);
    }
    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    
    bzero(&addr1, sizeof(struct sockaddr_un));
    addr1.sun_family = AF_LOCAL;
    strncpy(addr1.sun_path, argv[1], sizeof(addr1.sun_path) - 1);
    unlink(argv[1]);
    printf("SUN_LEN(&addr1)=%ld\n", SUN_LEN(&addr1));
    rc = bind(fd, (struct sockaddr *)&addr1, SUN_LEN(&addr1));

    if (rc < 0) {
        printf("bind error: %s\n", strerror(errno));
        exit(-1);
    }
    printf("len1=%d\n", (int)(((struct sockaddr_un *)0)->sun_path));
    printf("len1=%d\n", (int)&(((struct tyx_s *)0)->b));
    //printf("len1=%d\n", ((struct tyx_s *)0)->b);
    len = sizeof(addr2);

    getsockname(fd, (struct sockaddr *)&addr2, &len);

    printf("addr2.sun_path=%s, len=%d\n", addr2.sun_path, len);

    return 0;
}

