#pragma once

#include <math.h>
#include <time.h>

#define QUEUE_SIZE 128
#define MAX 65536

typedef struct Job {
  int priority;
  int timestamp;
  char *message;
} Job;

typedef struct BHeap {
  Job **jobs;
  int next_index;
  int max_index;
} BHeap;

Job *delete_min(BHeap *bheap);
void insert(BHeap *bheap, Job *job);
int left_child(int i);
int right_child(int i);
int parent(int i);
void double_queue(BHeap *bheap);
BHeap *init_bheap();
void swap(Job **jobs, int parent, int child);
