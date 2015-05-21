#include "sankaque.h"
#include "pq.h"
#include "test.h"


int random_priority() {
  int r = rand() % 5;

  return r;
}

char *random_message(int len) {
  char *msg = malloc(sizeof(char) * len);

  static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz(){}<>., ";

  int i = 0;

  for (i = 0; i < (len - 1); i++) {
    msg[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  msg[len-1] = '\0';

  return msg;
}

Job *random_job(BHeap *bheap) {
  Job *job = malloc(sizeof(struct Job));
  job->priority = random_priority();
  job->message = random_message(10);
  insert(bheap, job);

  return job;
}

void print_job(Job *job) {
  printf("pri: %d\t", job->priority);
  printf("message: %s\t", job->message);
  printf("timestamp: %d\n", job->timestamp);
}

void print_string(char *str) {
  int i;

  for (i = 0; i < strlen(str); i++) {
    printf("str[%d]: %c %d\n", i, str[i], str[i]);
  }
}
