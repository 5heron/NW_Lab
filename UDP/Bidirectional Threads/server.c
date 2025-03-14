#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100

struct sockaddr_in client_addr;
socklen_t addr_len = sizeof(client_addr);
pthread_t send_thread, recv_thread;

void *receive_messages(void *arg) {
    int sockfd = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recvfrom(sockfd, &buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len);
        if (bytes <= 0 || strcmp(buffer, "exit\n") == 0) {
            printf("\nConnection closed. Exiting...\n");
            pthread_cancel(send_thread);  // Stop send thread
            break;
        }
        printf("\nMessage Received : %s", buffer);
        printf("Message to send : ");  // Redisplay prompt for better UI
        fflush(stdout);
    }
    return NULL;
}

void *send_messages(void *arg) {
    int sockfd = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("Message to send : ");
        fflush(stdout);  // Ensure prompt appears immediately
        fgets(buffer, BUFFER_SIZE, stdin);
        sendto(sockfd, &buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
        if (strcmp(buffer, "exit\n") == 0) {
            printf("Exiting chat...\n");
            pthread_cancel(recv_thread);
            break;
        }
    }
    return NULL;
}

int setup_socket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Bind failed\n");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int main() {
    int sockfd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    sockfd = setup_socket();
    if (sockfd < 0) exit(EXIT_FAILURE);
    printf("Server listening on port %d...\n", PORT);
    pthread_create(&recv_thread, NULL, receive_messages, &sockfd);
    pthread_create(&send_thread, NULL, send_messages, &sockfd);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);

    close(sockfd);
    return 0;
}