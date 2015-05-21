#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#pragma once

int random_priority();
char *random_message();
void print_job(Job *job);
Job *random_job(BHeap *bheap);
void print_string(char *str);
