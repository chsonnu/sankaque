#include <stdio.h>
#include <string.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include "pq.h"
#include "test.h"

int main(int argc, char *argv[]) {
  srand(time(NULL));

  if (!argv[1] || !argv[2] || !argv[3]) 
    return 1;

  char *op = argv[1];
  int delay, iterations, i;
  sscanf(argv[2], "%d", &delay);
  sscanf(argv[3], "%d", &iterations);
  
  int socket_desc;
  struct sockaddr_in server;
  char message[2000];
  char server_reply[2000];

  socket_desc = socket(AF_INET, SOCK_STREAM, 0);    

  if (socket_desc == -1) {
    printf("Could not create socket");
  }

  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_family = AF_INET;
  server.sin_port = htons( 12000 );
 
  //Connect to remote server
  if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0) {
    puts("connect error");
    return 1;
  }

  puts("Connected");

  for (i = 0; i < iterations; i++) {
    if (strcmp(op, "push") == 0) {
      snprintf(message, sizeof(message), "sank %s %d %s\r\n", op, random_priority(), random_message(10));
    } else if ((strcmp(op, "pop") == 0) || (strcmp(op, "stats") == 0) || (strcmp(op, "purge") == 0)) {
      snprintf(message, sizeof(message), "sank %s\r\n", op);
    }

    printf("\nRequest: %s\n", message);

    if (send(socket_desc, message, strlen(message), 0) < 0) {
      puts("Send failed");
      return -1;
    }

    if (recv(socket_desc, server_reply, 2000, 0) < 0)
      puts("recv failed");

    //print_string(server_reply);

    printf("%s\n", server_reply);
    printf("strlen(server_reply): %zu\n", strlen(server_reply));
    printf("sizeof(server_reply): %zu\n", sizeof(server_reply));

    memset(server_reply, 0, strlen(server_reply));

    usleep(delay);
  }

  close(socket_desc);

  return 0;
}
