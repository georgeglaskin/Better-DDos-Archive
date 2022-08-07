#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include "headers/main.h"
#include "headers/epoll.h"
#include "headers/connection.h"

int efd;

void epoll_control(int fd, int op, uint32_t events) {
    struct epoll_event ev = {
        events, 
        .data.fd = fd
    };

    epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev);
}

void *epoll_listener(void *args) {
    int nfds, fd, ret, fake_time;
    struct epoll_event *events = calloc(MAX_FDS, sizeof(struct epoll_event));

    while(1) {
        nfds = epoll_wait(efd, events, MAX_FDS, -1);

        if(nfds == -1)
            perror("epoll listener");

        for(int i = 0; i < nfds; i++) {
            fd = events[i].data.fd;

            if(events[i].events & EPOLLOUT)
                check_connection(fd);
            else if(events[i].events & (EPOLLIN | EPOLLET))
                handle_connection(&fd_info[fd]);
            else
                exit_fd(fd);
        }
    }
}

void epoll_init(void) {
    if((efd = epoll_create1(0)) == -1) {
        perror("epoll fd");

        return;
    }
}
