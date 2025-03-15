#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define BACKLOG 5  // Max pending connections

int setup_socket() {
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

void send_file(FILE *fp, int client_sock) {
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
        send(client_sock, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUFFER_SIZE);
    }
    printf("[SERVER] File sent successfully.\n");
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in client_addr;
    socklen_t client_size;
    char filename[BUFFER_SIZE];

    server_sock = setup_socket();
    printf("[SERVER] Waiting for connections...\n");

    client_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_size);
    printf("[SERVER] Client connected.\n");

    recv(client_sock, filename, BUFFER_SIZE, 0);
    printf("[SERVER] Client requested file: %s\n", filename);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("[SERVER] File not found.\n");
        send(client_sock, "File not found", 14, 0);
    } else {
        send_file(fp, client_sock);
        fclose(fp);
    }

    close(client_sock);
    close(server_sock);
    return 0;
}
