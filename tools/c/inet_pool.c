#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int total_addrs = 0;
in_addr_t *addr_arrays[2];

int inet_find(in_addr_t *addr_array) {
    for(int i = 0; i < total_addrs; i++) {
        if(addr_array[i] == 0)
            return i;
    }
    return -1;
}

void inet_append(in_addr_t *addr_array, in_addr_t a_addr) {
    int spot;
    if((spot = inet_find(addr_array)) == -1) {
        addr_array = realloc(addr_array, (total_addrs + 1) * sizeof(in_addr_t));
        addr_array[total_addrs++] = a_addr;
    }
    else
        addr_array[spot] = a_addr;
}

void inet_remove(in_addr_t *addr_array, in_addr_t r_addr) {
    int i;
    for(i = 0; i < total_addrs; i++) {
        if(r_addr == addr_array[i])
            goto remove;
    }
    
    return;

remove:
    memset(&addr_array[i], 0, sizeof(in_addr_t));
    total_addrs--;
}

int main(void) {
    srand(time(NULL));

    addr_arrays[0] = (in_addr_t *)malloc(sizeof(in_addr_t *));

    in_addr_t *addr_array = addr_arrays[0];
    
    inet_append(addr_array, inet_addr("1.1.1.1"));

    printf("addr_arry[0] = %d\n", addr_array[0]);
    printf("addr_arry[1] = %d\n", addr_array[1]);

    inet_append(addr_array, inet_addr("2.2.2.2"));

    puts("");

    printf("addr_arry[0] = %d\n", addr_array[0]);
    printf("addr_arry[1] = %d\n", addr_array[1]);

    inet_remove(addr_array, inet_addr("1.1.1.1"));

    puts("");

    printf("addr_arry[0] = %d\n", addr_array[0]);
    printf("addr_arry[1] = %d\n", addr_array[1]);

    inet_append(addr_array, inet_addr("1.1.1.1"));

    puts("");

    printf("addr_arry[0] = %d\n", addr_array[0]);
    printf("addr_arry[1] = %d\n", addr_array[1]);
}
