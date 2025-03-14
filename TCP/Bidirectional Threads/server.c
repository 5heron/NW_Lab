#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100
#define BACKLOG 5  // Max pending connections

pthread_t send_thread, recv_thread;

void *receive_messages(void *arg) {
    int client_fd = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes <= 0 || strcmp(buffer, "exit\n") == 0) {
            printf("\nConnection closed. Exiting...\n");
            pthread_cancel(send_thread);  // Stop send thread
            break;
        }
        printf("\nMessage Received : %s", buffer);
        printf("Message to send : ");  // Redisplay prompt for better UI
        fflush(stdout);
    }
    close(client_fd);
    return NULL;
}

void *send_messages(void *arg) {
    int client_fd = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("Message to send : ");
        fflush(stdout);  // Ensure prompt appears immediately
        fgets(buffer, BUFFER_SIZE, stdin);
        send(client_fd, buffer, BUFFER_SIZE, 0);
        if (strcmp(buffer, "exit\n") == 0) {
            printf("Exiting chat...\n");
            break;
        }
    }
    close(client_fd);
    return NULL;
}


int setup_socket() {
    int sockfd, opt = 1;
    struct sockaddr_in addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); //To free port when client exit, allow port reuse.
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
    server_fd = setup_socket(); // setup TCP server socket
    if (server_fd < 0) exit(EXIT_FAILURE);
    printf("Server listening on port %d...\n", PORT);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        printf("Client accept failed\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Client connected\n");
    pthread_create(&recv_thread, NULL, receive_messages, &client_fd);
    pthread_create(&send_thread, NULL, send_messages, &client_fd);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);

    close(server_fd);
    return 0;
}