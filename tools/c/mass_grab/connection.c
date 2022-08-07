#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include "headers/main.h"
#include "headers/tools.h"
#include "headers/epoll.h"
#include "headers/debug.h"
#include "headers/connection.h"

void exit_fd(int fd) {
    epoll_control(fd, EPOLL_CTL_DEL, EPOLLOUT);
    memset(&fd_info[fd], 0, sizeof(info_t));
    close(fd);

    ATOMIC_DEC(&running);
}

void start_connection(in_addr_t host, int port) {
    ATOMIC_INC(&running);

    int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if(fd == -1) {
        ATOMIC_DEC(&running);

        return;
    }
    
    struct sockaddr_in addr = {
        .sin_addr.s_addr = host,
        .sin_port = htons(port),
        .sin_family = AF_INET
    };

    connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

#ifdef DEBUG
    PRINTF("started connection %s:%d [%d]\n", inet_ntoa((struct in_addr){host}), port, fd);
#endif

    info_t *info = &fd_info[fd];

    info->addr = host;
    info->port = port;
    info->fd = fd;

    epoll_control(fd, EPOLL_CTL_ADD, EPOLLIN | EPOLLET);
}

void check_connection(int fd) {
    int err = 0, ret;
    socklen_t err_len = sizeof(int);

    ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &err_len);

    if(!ret && !err) {
#ifdef DEBUG
        PRINTF("successfull connection %s:%d [%d]\n", inet_ntoa((struct in_addr){fd_info[fd].addr}), fd_info[fd].port, fd);
#endif

        epoll_control(fd, EPOLL_CTL_MOD, EPOLLIN | EPOLLET);
    }
    else {
#ifdef DEBUG
        PRINTF("unsuccessfull connection %s:%d [%d] [%s]\n", inet_ntoa((struct in_addr){fd_info[fd].addr}), fd, fd_info[fd].port, strerror(errno));
#endif
        exit_fd(fd);
    }
}

void handle_connection(info_t *info) {
#ifdef DEBUG
    PRINTF("handling connection %s:%d [%d]\n", inet_ntoa((struct in_addr){info->addr}), info->port, info->fd);
#endif

    char request[2048] = {0}, rdbuf[512] = {0};
    sprintf(request, "GET / HTTP/1.1\r\nHost: %s\r\nUser-Agent: Wget\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\n", inet_ntoa((struct in_addr){info->addr}));

    send(info->fd, request, strlen(request), MSG_NOSIGNAL);
    recv(info->fd, rdbuf, 512, MSG_NOSIGNAL);

    if(strlen(rdbuf) > 0) {
#ifdef PRINT_RESP
        printf("bytes [%ld]\r\n%s\r\n", strlen(rdbuf), rdbuf);
#endif
        if(strcasestr(rdbuf, s_string)) {
            ATOMIC_INC(&found);

            send_log("grab_found.txt", "%s:%d\r\n", inet_ntoa((struct in_addr){info->addr}), info->port);
        }
    }
    

    exit_fd(info->fd);
    ATOMIC_INC(&ran);
}
