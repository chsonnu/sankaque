#pragma once

#include "pq.h"
#include <pthread.h> 

#define MAX 65536

#define OP_MUTEX_ERROR -2
#define OP_UNKNOWN -1
#define OP_PUSH 1
#define OP_POP 2
#define OP_STATS 3
#define OP_PURGE 4

typedef struct ThreadParams {
  int socket;
  BHeap *bheap;
} ThreadParams;

void intHandler(int sig);
void *connection_handler(void *arg);
int parse_message(char *msg, int sock);
typedef void (*fptrBase)();
