#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "headers/tools.h"
#include "headers/epoll.h"
#include "headers/debug.h"
#include "headers/connection.h"

int total_addrs = 0, port;

void *writer(void *args) {
    int seconds = 0;

    while(1) {
        PRINTF("[%d's] total [%d] running [%d] found [%d] ran [%d]\n", ATOMIC_INC(&seconds), ATOMIC_GET(&total), ATOMIC_GET(&running), ATOMIC_GET(&found), ATOMIC_GET(&ran));

        sleep(1);
    }
}

int main(int argc, char **argv) {
#ifdef DEBUG2
    if(argc != 5) {
#else
    if(argc != 4) {
#endif
        PRINTF("usage: %s [threads] [port] [string]\n", argv[0]);

        return 0;
    }

#ifdef DEBUG2
    FILE *fp = fopen(argv[4], "r");
#endif

    epoll_init();
    strncpy(s_string, argv[3], sizeof(s_string));

    int threads = atoi(argv[1]);
    port = atoi(argv[2]);

    pthread_t thread[threads + 2];

    char rdbuf[16];
    pthread_create(&thread[threads + 1], NULL, writer, NULL);
    pthread_create(&thread[threads + 2], NULL, epoll_listener, NULL);

    while(1) {
        memset(rdbuf, 0, 16);

        if(!read(STDIN_FILENO, rdbuf, sizeof(rdbuf)))
            break;
        if(!strlen(rdbuf))
            continue;
        
        trim(rdbuf);
        start_connection(inet_addr(rdbuf), port);

        ATOMIC_INC(&total);
    }

    for(int i = 0; i < threads; i++)
        pthread_create(&thread[i], NULL, epoll_listener, NULL);
    
    puts("done reading");
    
    while(1)
        sleep(1);
}
