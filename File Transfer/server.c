#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 1024
#define SOCKET_ERROR (-1)
#define MAX_CLIENTS 10

int check(int exp, const char *msg) {
  if (exp == SOCKET_ERROR) {
    perror(msg);
    exit(EXIT_FAILURE);
  }
  return exp;
}

void *handle_client(void *arg) {
  int client_sock = *(int *)arg;
  free(arg);
  char filename[256] = {0};
  int bytes_read = check(read(client_sock, filename, sizeof(filename) - 1),
                         "Error reading filename from client");
  filename[bytes_read] = '\0';
  printf("Client requested file: %s\n", filename);

  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    char *error_msg = "File not found";
    write(client_sock, error_msg, strlen(error_msg));
  } else {
    char buffer[BUF_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
      if (write(client_sock, buffer, bytes) < 0) {
        perror("Error sending file content");
        break;
      }
    }
    fclose(fp);
  }
  close(client_sock);
  pthread_exit(NULL);
}

int main() {
  int server_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);
  server_sock =
      check(socket(AF_INET, SOCK_STREAM, 0), "Socket creation failed!");

  int opt = 1;
  setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  check(bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)),
        "Socket binding failed!");

  check(listen(server_sock, 5), "Listening failed!");
  printf("Server listening on port %d...\n", PORT);

  int next_client = 0;
  pthread_t clients[MAX_CLIENTS];
  while (1) {
    int *client_sock = malloc(sizeof(int));
    *client_sock =
        check(accept(server_sock, (struct sockaddr *)&client_addr, &client_len),
              "Failed to accept connection!");

    if (pthread_create(&clients[next_client], NULL, handle_client,
                       client_sock) != 0) {
      perror("Error creating thread");
      close(*client_sock);
      free(client_sock);
      continue;
    }
    pthread_detach(clients[next_client]);
    next_client++;
  }
  return 0;
}