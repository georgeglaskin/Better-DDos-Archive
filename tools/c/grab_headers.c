#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

void make_request(char *buffer, in_addr_t host) { //in case we want to string copy
    sprintf(buffer, "GET / HTTP/1.1\r\nHost: %s\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n", inet_ntoa((struct in_addr){host}));
}

void handle_fd(int fd, in_addr_t host) {
    char buf[4096] = {0}, rdbuf[4096] = {0};

    make_request(buf, host);

    write(fd, buf, strlen(buf));

    read(fd, rdbuf, 4096);

    puts("START");
    puts(rdbuf);
    puts("END");
}

void connection_init(in_addr_t host, int port) {
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        return;
    
    struct sockaddr_in addr = {
        .sin_addr.s_addr = host,
        .sin_port = htons(port),
        .sin_family = AF_INET
    };

    if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
        perror("connect");

        return;
    }

    handle_fd(sock, host);
}

int main(int argc, char **argv) {
    connection_init(inet_addr(argv[1]), atoi(argv[2]));
}
