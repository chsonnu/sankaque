#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>  
#include <signal.h>
#include <stdbool.h>
#include "sankaque.h"
#include "pq.h"
#include "test.h"
#include "cli.h"

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static BHeap *bheap;
const char *delim = " ";

void intHandler(int sig) {  
  printf("sig: %d\n", sig);
  free(bheap->jobs);
  free(bheap);

  exit(0);
}

void print_heap(BHeap *bheap, int sock) {
  int i = 0;
  Job *job;

  for (i = 0; i < bheap->next_index; i++) {
    // todo: allocate correct size
    char buffer[MAX];
    job = bheap->jobs[i];
    sprintf(buffer, 
	    "%d\t%d\t%s\t%d\n", 
	    i, 
	    job->priority, 
	    job->message, 
	    job->timestamp);
    respond(sock, buffer, false);
  }
}

void respond(int sock, char *message, bool final) {
  char *tail;
  
  if (final) {
    tail = "\r\n\r\n";
  } else {
    tail = "\r\n";
  }

  char buffer[strlen(message) + strlen(tail) + 1];

  // sprintf(buffer, "%s\r\n", message);
  sprintf(buffer, "%s%s", message, tail);
  write(sock, buffer, strlen(buffer) + 1);
}

int parse_message(char *msg, int sock) {
  int op, m, i, jobs;
  char *p, response[MAX];;
  Job *job;

  if ((msg[strlen(msg) - 1] != '\n') && (msg[strlen(msg) - 2] != '\r')) 
    return OP_UNKNOWN;;

  // msg[strlen(msg) - 2] = 0;

  p = strtok(msg, delim);

  if (strcmp(p, "sank") != 0) 
    return OP_UNKNOWN;

  p = strtok(NULL, delim);

  if (strcmp(p, "push") == 0) {
    char *msg_body;
    long priority;
    time_t seconds;

    p = strtok(NULL, delim);
    priority = strtol(p, NULL, 10);

    p = strtok(NULL, "\r\n");
    msg_body = strdup(p);

    if (msg_body == 0) 
      return OP_UNKNOWN;

    seconds = time(NULL);

    job = malloc(sizeof(struct Job));
    job->priority = priority;
    job->message = msg_body;
    job->timestamp = seconds;

    if (pthread_mutex_lock(&mtx) != 0) return OP_MUTEX_ERROR;
    insert(bheap, job);
    if (pthread_mutex_unlock(&mtx) != 0) return OP_MUTEX_ERROR;

    strcpy(response, "ACK");
    op = OP_PUSH;
  } else if (strcmp(p, "pop\r\n") == 0) {
    if (pthread_mutex_lock(&mtx) != 0) return OP_MUTEX_ERROR;
    job = delete_min(bheap);
    if (pthread_mutex_unlock(&mtx) != 0) return OP_MUTEX_ERROR;

    if (job) {
      char priority[15];
      sprintf(priority, "%d ", job->priority);

      strcpy(response, priority);
      strcat(response, job->message);

      free(job->message);
      free(job);
    } else {
      strcpy(response, "QUEUE EMPTY");
    }

    op = OP_POP;
  } else if (strcmp(p, "stats\r\n") == 0) {
    char count[30];

    sprintf(count, "JOBS IN QUEUE: %d", bheap->next_index);
    strcpy(response, count);

    op = OP_STATS;
  } else if (strcmp(p, "print\r\n") == 0) {
    respond(sock, "#\tPriority\tMessage\tTimestamp", false);
    print_heap(bheap, sock);
    strcpy(response, "---");

    op = OP_PRINT;
  } else if (strcmp(p, "purge\r\n") == 0) {
    if (pthread_mutex_lock(&mtx) != 0) return OP_MUTEX_ERROR;

    if (bheap->next_index > 0) {
      jobs = bheap->next_index;
      for (i = 0; i < jobs; i++) {
	job = delete_min(bheap);
	free(job->message);
	free(job);
      }
    }

    if (pthread_mutex_unlock(&mtx) != 0) return OP_MUTEX_ERROR;

    strcpy(response, "QUEUE PURGED");

    op = OP_PURGE;
  } else {
    op = OP_UNKNOWN;
  }

  respond(sock, response, true);

  return op;
}

void *connection_handler(void *arg) {
  ThreadParams *thread_params = arg;
  int sock = thread_params->socket, read_size, parse_response;
  char request[MAX];

  while ((read_size = recv(sock, request, MAX, 0)) > 0 ) {
    if ((parse_response = parse_message(request, sock)) > 0) {
      
    } else if (parse_response == OP_UNKNOWN) {
      respond(sock, "OPERATION UNRECOGNIZABLE", true);
    } else if (parse_response == OP_MUTEX_ERROR) {
      respond(sock, "MUTEX COULD NOT BE LOCKED/UNLOCKED", true);
    }

    memset(request, 0, strlen(request));
  }

  //print_heap(bheap);
  free(thread_params);

  if (read_size == 0) {
    puts("Client disconnected");
    fflush(stdout);
  } else if (read_size == -1) {
    perror("Recv failed");
  }
              
  return NULL;
}

int main(int argc, char *argv[]) {
  CLIOptions *options = init_cli();
  parse_cli(argc, argv, options);

  char *address = options->address;
  uint16_t port = options->port;

  printf("Sankaque listening on..\nAddress: %s\nPort: %d\n", address, port);

  signal(SIGPIPE, SIG_IGN);
  signal(SIGINT, intHandler);

  srand(time(NULL));
  bheap = init_bheap();

  int socket_desc, new_socket, c;
  struct sockaddr_in server, client;
     
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc == -1) {
    printf("Could not create socket");

    return 1;
  }
     
  server.sin_family = AF_INET;
  inet_aton(address, &server.sin_addr);

  // server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

  //server.sin_addr.s_addr = aaddress;
  //server.sin_port = htons(pport);

  if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
    puts("Bind failed");

    return 1;
  }

  //puts("bind done");

  listen(socket_desc, 128);

  puts("Waiting for incoming connections...");
  c = sizeof(struct sockaddr_in);

  while ((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c))) {
    puts("Connection accepted");

    pthread_t thread;
    
    ThreadParams *thread_params = malloc(sizeof(ThreadParams));
    thread_params->socket = new_socket;
    thread_params->bheap = bheap;

    if (pthread_create(&thread, NULL, connection_handler, thread_params) < 0) {
      perror("could not create thread");

      return 1;
    }

    if (pthread_detach(thread) < 0) 
      perror("Could not detach thread");   

    puts("Handler assigned");
  }

  if (new_socket < 0) {
    perror("accept failed");

    return 1;
  }

  return 0;
}
