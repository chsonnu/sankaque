#include "cli.h"

void print_usage(char *appname) {
  printf("Usage: %s -l ADDR -p PORT\n", appname);
}

void parse_cli(int argc, char *argv[], CLIOptions *options) {
  int option = 0;

  while ((option = getopt(argc, argv,"l:p:e:")) != -1) {
    switch (option) {
    case 'l': 
      // error checking
      strcpy(options->address, optarg);
      break;
    case 'p': 
      // error checking
      options->port = (uint16_t) atoi(optarg);
      break;
    case 'e':
      options->command = optarg;
      break;
    default: print_usage(argv[0]);
      exit(EXIT_FAILURE);
    }
  }
}

CLIOptions *init_cli() {
  CLIOptions *options = malloc(sizeof(struct CLIOptions));
  strcpy(options->address, "0.0.0.0");
  options->port = 12000;

  return options;
}
