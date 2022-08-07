#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

char *read_path(char *path) {
    char buf[256], *rdbuf = buf;

    int fd;
    if((fd = open(path, O_RDONLY)) == -1)
        return NULL;

    read(fd, rdbuf, sizeof(buf) - 1);

    return rdbuf;
}

int main() {
    char buf[256] = {0};

    char path[] = "/proc/self/loginuid";

    char *rbuf = read_path(path);

    readlink(path, buf, sizeof(buf) - 1);

    printf("read_link: %s read: %s\n", buf, rbuf);

    struct passwd *pws = getpwuid(atoi(rbuf));
    if(pws == NULL) {
        printf("%d %s\n", errno, strerror(errno));

        return -1;
    }

    printf("%s\n", pws->pw_name);
}
