#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 1024
#define SOCKET_ERROR (-1)

int check(int exp, const char *msg) {
  if (exp == SOCKET_ERROR) {
    perror(msg);
    exit(EXIT_FAILURE);
  }
  return exp;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int sock = check(socket(AF_INET, SOCK_STREAM, 0), "Error creating socket!");
  struct sockaddr_in server_addr;
  char filename[256];
  char buffer[BUF_SIZE];

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
    perror("Invalid address/ Address not supported");
    close(sock);
    exit(EXIT_FAILURE);
  }

  check(connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)),
        "Failed to connect");

  printf("Enter the filename to request: ");
  if (fgets(filename, sizeof(filename), stdin) == NULL) {
    fprintf(stderr, "Error reading filename\n");
    close(sock);
    exit(EXIT_FAILURE);
  }
  filename[strcspn(filename, "\n")] = '\0';

  send(sock, &filename, sizeof(filename) + 1, 0);
  int bytes_read;
  while ((bytes_read = check(read(sock, buffer, sizeof(buffer) - 1),
                             "Error receiving file contents from server")) >
         0) {
    buffer[bytes_read] = '\0';
    printf("%s\n", buffer);
  }
  close(sock);
  return 0;
}