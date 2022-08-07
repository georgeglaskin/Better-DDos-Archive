#include <stdio.h>
#include <string.h>

int starts_with(char *ss1, char *ss2) {
    char *s1 = ss1, *s2 = ss2;
    int len2 = strlen(s2);

    while(len2--) {
        if(*s1++ != *s2++)
            return 1;
    }
    return 0;
}

int main() {
    char *s1 = "hello", *s2 = "hell";

    if(!starts_with(s1, s2))
        printf("found\n");
    else
        printf("not found\n");
}
