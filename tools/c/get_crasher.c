#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

void accept_print(char *host) {
    printf("Twas this!? Another Faggot!!! [%s]\n", host);
}

void accept_main(int fd, struct sockaddr_in addr) {
    accept_print(inet_ntoa((struct in_addr){addr.sin_addr.s_addr}));

    write(fd, "got your ip monkeeey\r\n", 22);

    close(fd);
}

void accept_loop(int sock) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    while(1) {
        usleep(20000);
        
        int fd;
        if((fd = accept(sock, (struct sockaddr *)&addr, &len)) == -1)
            continue;
        
        accept_main(fd, addr);
    }
}

int init_fd(void) {
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("socket");

        return -1;
    }
    
    struct sockaddr_in addr = {
        .sin_addr.s_addr = inet_addr("0.0.0.0"),
        .sin_port = htons(7777),
        .sin_family = AF_INET
    };

    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");

        return -1;
    }
    else if(listen(sock, 10940) == -1) {
        perror("listen");
        
        return -1;
    }
}

int main(void) {
    int fd;
    if((fd = init_fd()) == -1)
        return -1;
    
    accept_loop(fd);
}
