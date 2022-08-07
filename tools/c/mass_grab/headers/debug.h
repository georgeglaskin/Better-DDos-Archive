#pragma once

#include <errno.h>
#include <stdio.h>

#define PRINTF(args, ...) printf("[%s] " args, __FILE__, ##__VA_ARGS__)
