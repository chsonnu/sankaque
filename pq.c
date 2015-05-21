#include <stdio.h>
#include <stdlib.h>
#include "pq.h"
#include "test.h"

/*
#define left_child(a) ((2 * a) + 1)
#define right_child(a) ((2 * a) + 2)
#define parent(a) ((a - 1) / 2)
*/

int left_child(int i) {
  int index = (2 * i) + 1;
   
  return index;
}

int right_child(int i) {
  int index = (2 * i) + 2;

  return index;
}

int parent(int i) {
  int index = (i - 1) / 2;
  index = (int)floor(index);

  return index;
}

void swap(Job **jobs, int parent, int child) {
  Job *temp;

  temp = jobs[parent];
  jobs[parent] = jobs[child];
  jobs[child] = temp;
}

void insert(BHeap *bheap, Job *job) {
  int next_index = bheap->next_index;
  
  if (next_index > bheap->max_index)
    double_queue(bheap);

  bheap->jobs[next_index] = job;

  if (next_index > 0) {
    int parent_index = parent(next_index);
    int child_index = next_index;

    while (bheap->jobs[parent_index]->priority > job->priority) {
      swap(bheap->jobs, parent_index, child_index);
      child_index = parent_index;
      parent_index = parent(parent_index);
    }
  } else if (next_index == 0) {
    bheap->jobs[next_index] = job;
  }
  
  bheap->next_index++;
}

Job *delete_min(BHeap *bheap) {
  if (bheap->next_index > 0) {
    int parent_index = 0;
    Job *min_job = bheap->jobs[parent_index];

    bheap->jobs[parent_index] = bheap->jobs[(bheap->next_index - 1)];
    bheap->jobs[(bheap->next_index - 1)] = NULL;
    bheap->next_index--;

    int left_index = left_child(parent_index);
    int right_index = right_child(parent_index);

    if (bheap->next_index > 2) {
      int child_index;

      while((bheap->jobs[parent_index]->priority > bheap->jobs[left_index]->priority) || (bheap->jobs[parent_index]->priority > bheap->jobs[right_index]->priority)) {
	if ((bheap->jobs[left_index]->priority < bheap->jobs[right_index]->priority) || (bheap->jobs[left_index]->priority == bheap->jobs[right_index]->priority))
	  child_index = left_index;
	else if (bheap->jobs[right_index]->priority < bheap->jobs[left_index]->priority)
	  child_index = right_index;	
      
	swap(bheap->jobs, parent_index, child_index);
	parent_index = child_index;
	left_index = left_child(parent_index);
	right_index = right_child(parent_index);
      
	if (left_index >= bheap->next_index || right_index >= bheap->next_index)
	  break;
      }
    }
    
    return min_job;
  } else {
    return NULL;
  }
}

void double_queue(BHeap *bheap) {
  int new_max_index = bheap->max_index * 2;
  Job **temp = realloc(bheap->jobs, sizeof(Job) * (new_max_index + 1));

  if (temp) {
    bheap->jobs = temp;
    bheap->max_index = new_max_index;
  }
  else {
    // TODO: Save queue to binlog.
    exit(1);
  }
}

BHeap *init_bheap() {
  BHeap *bheap = malloc(sizeof(struct BHeap));
  bheap->jobs = malloc(sizeof(struct Job) * QUEUE_SIZE);
  bheap->next_index = 0;
  bheap->max_index = QUEUE_SIZE - 1;

  return bheap;
}
