#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>

int get_ram(void) {
    struct sysinfo info;

    sysinfo(&info);

    return info.totalram *(unsigned long long)in.mem_unit / 1024;
}

int main(void) {
    printf("%d\n", get_ram());
}
