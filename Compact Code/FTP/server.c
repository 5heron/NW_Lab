// server.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 1024

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { AF_INET, htons(PORT), INADDR_ANY };

    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 5);

    int client_fd = accept(sockfd, NULL, NULL);
    printf("Client connected.\n");

    char filename[BUFFER_SIZE], filedata[BUFFER_SIZE];
    memset(filename, 0, BUFFER_SIZE);

    // Receive filename from client
    int bytes = recv(client_fd, filename, BUFFER_SIZE, 0);
    filename[bytes] = '\0';
    printf("Client requested file: %s\n", filename);

    // Try to open file
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        strcpy(filedata, "No such file");
        send(client_fd, filedata, strlen(filedata), 0);
    } else {
        // Read and send file content
        memset(filedata, 0, BUFFER_SIZE);
        int n = read(fd, filedata, BUFFER_SIZE);
        send(client_fd, filedata, n, 0);
        close(fd);
    }

    close(client_fd);
    close(sockfd);
    return 0;
}