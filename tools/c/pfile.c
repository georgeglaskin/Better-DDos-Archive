#ifdef DEBUG
#define PRINTF(args, ...) printf("[%s] " args, FILE, ##__VA_ARGS__)
#else
#define PRINTF(args, ...)
#endif