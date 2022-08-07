#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

int total_open = 0;

typedef struct arginfo_t {
    in_addr_t host;

    int port;
} args_t;

void print_total(void) {
    while(1) {
        printf("[main] total open ports [%d]\n", total_open);

        sleep(1);
    }
}

void check_port(void *arg) {
    args_t *args = (args_t *)arg;

    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
#ifdef DEBUG
        printf("[scanner] sock error [%s]\n", strerror(errno));
#endif

        return;
    }
    
    struct sockaddr_in {
        .sin_addr.s_addr = args->host;
        .sin_port = htons(args->port);
        .sin_family = AF_INET;
    };

    if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) > -1) {
        total_open++;

        printf("[scanner] open port [%d]\n", args->port);
    }
    else {
#ifdef DEBUG
        printf("[scanner] closed port [%d] [%s]\n", args->port, strerror(errno));
#endif
    }
}

void thread_init(in_addr_t addr, int port, pthread_t *thread_table) {
    args_t args;

    args.port = port;
    args.host = addr;

    pthread_create(&thread_table[args.port], NULL, check_port, (void *)&args);
    pthread_join(thread_table[port], NULL);
}

int main(int argc, char **argv) {
    in_addr_t addr;

    if(argc != 4) {
        printf("[main] usage: %s host threads max\n", argv[0]);

        return 0;
    }

    else if((addr = inet_addr(argv[1])) < 1) {
        puts("[main] invalid host");

        return 0;
    }

    else if(atoi(argv[2]) < 1) {
        puts("[main] invalid threads");

        return 0;
    }

    else if(atoi(argv[3]) < 1) {
        puts("[main] invalid max port");

        return 0;
    }

    pthread_t args[atoi(argv[3])];

    int total = 0;

    for(total = 0; total < atoi(argv[3]); total += threads) {
        for(int i = 0; i < threads; i++)
            thread_init(addr, total++, args);
        
        sleep(1);
    }
}
