#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(void)
{
    struct hostent *hp = gethostbyname("nigger.com");
    printf("ip: (%s)\n", inet_ntoa(*(struct in_addr *)hp->h_addr));

    return 0;
}