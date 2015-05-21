#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct CLIOptions {
  char address[16];
  uint16_t port;
  char *command;
} CLIOptions;

void print_usage(char *appname);
void parse_cli(int argc, char *argv[], CLIOptions *options);
CLIOptions *init_cli();

