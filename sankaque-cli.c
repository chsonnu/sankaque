#include <stdio.h>
#include <string.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include "pq.h"
#include "test.h"
#include "cli.h"

int main(int argc, char *argv[]) {
  CLIOptions *options = init_cli();
  parse_cli(argc, argv, options);
  int socket_desc, read_size;
  struct sockaddr_in server;
  char message[2000], server_reply[2000];
  srand(time(NULL));

  socket_desc = socket(AF_INET, SOCK_STREAM, 0);    

  if (socket_desc == -1) puts("Could not create socket");

  server.sin_addr.s_addr = inet_addr(options->address);
  server.sin_family = AF_INET;
  server.sin_port = htons(options->port);
 
  if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0) {
    puts("Connect error");

    return 1;
  }

  snprintf(message, sizeof(message), "sank %s\r\n", options->command);

  // for (i = 0; i < iterations; i++) {
  /*
    if (strcmp(op, "push") == 0) {
      snprintf(message, sizeof(message), "sank %s %d %s\r\n", op, random_priority(), random_message(10));
    } else if ((strcmp(op, "pop") == 0) || (strcmp(op, "stats") == 0) || (strcmp(op, "purge") == 0)) {
      snprintf(message, sizeof(message), "sank %s\r\n", op);
    }
  */
    printf("\nRequest: %s\n", message);

    if (send(socket_desc, message, strlen(message), 0) < 0) {
      puts("Send failed");

      return -1;
    }

    //while ((read_size = recv(socket_desc, server_reply, MAX, 0) > 0)) {
    //  printf("read_size: %d\n", read_size);
    
    // while tail isn't \r\n\r\n

    if (recv(socket_desc, server_reply, 2000, 0) < 0)
      puts("recv failed");    
    
    printf("%s\n", server_reply);

    //printf("strlen(server_reply): %zu\n", strlen(server_reply));
    //printf("sizeof(server_reply): %zu\n", sizeof(server_reply));
    
    memset(server_reply, 0, strlen(server_reply));
 
    //usleep(delay);
    //}

    close(socket_desc);

  return 0;
}
