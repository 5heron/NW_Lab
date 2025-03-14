#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100
#define BACKLOG 5  // Max pending connections

int setup_socket(const char ip_addr[]) {
    int sockfd;
    struct sockaddr_in addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Bind failed\n");
        close(sockfd);
        return -1;
    }
    if (listen(sockfd, BACKLOG) < 0) {
        printf("Listen failed\n");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int main() {
    int server_fd, client_fd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    server_fd = setup_socket(NULL);
    if (server_fd < 0) exit(EXIT_FAILURE);
    printf("Server listening on port %d...\n", PORT);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        printf("Client accept failed\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Client connected\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);        
        if (recv(client_fd, buffer, BUFFER_SIZE, 0) > 0) {
            printf("Recieved message : %s", buffer);
            printf("Enter message to send : ");
            fgets(buffer, BUFFER_SIZE, stdin);
            send(client_fd, buffer, BUFFER_SIZE, 0);
        } else {
            printf("Client disconnected.\n");
            break;
        }
    }
    close(client_fd);
    close(server_fd);
}