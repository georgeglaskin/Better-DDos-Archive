#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

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

void send_log(char *file, char *msg, ...) {
    char buffer[1024];
    
    FILE *fp = fopen(file, "a");
    if((fp == NULL))
        return;

    va_list arglist;

    va_start(arglist, msg);
    vsnprintf(buffer, sizeof(buffer), msg, arglist);
    va_end(arglist);
    
    fprintf(fp, "%s", buffer);
    fclose(fp);
}
