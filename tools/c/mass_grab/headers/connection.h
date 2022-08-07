#pragma once

#include <fcntl.h>

#include "main.h"

#define SOCKET_TIMEOUT 1
#define NONBLOCK(fd) (fcntl(fd, F_SETFL, O_NONBLOCK))

void exit_fd(int);
void check_connection(int);
void handle_connection(info_t *);
void start_connection(in_addr_t, int);

