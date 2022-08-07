#pragma once

#include <stdint.h>

void epoll_init(void);
void *epoll_listener(void *);
void epoll_control(int, int, uint32_t);
