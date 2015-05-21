#pragma once

#include "pq.h"
#include <pthread.h> 

#define OP_MUTEX_ERROR -2
#define OP_UNKNOWN -1
#define OP_PUSH 1
#define OP_POP 2
#define OP_STATS 3
#define OP_PURGE 4
#define OP_PRINT 5

typedef struct ThreadParams {
  int socket;
  BHeap *bheap;
} ThreadParams;

void intHandler(int sig);
void *connection_handler(void *arg);
int parse_message(char *msg, int sock);
void respond(int sock, char *message, bool final);
void print_heap(BHeap *bheap, int sock);
