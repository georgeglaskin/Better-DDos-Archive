#pragma once

char **logins;

typedef struct login {
    char *user, *pass;

    struct login *prev, *next;
} login_t;

login_t *list_head = NULL;
