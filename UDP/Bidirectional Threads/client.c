#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100

pthread_t send_thread, recv_thread;
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);

void *receive_messages(void *arg) {
    int sockfd = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recvfrom(sockfd, &buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &addr_len);
        if (bytes <= 0 || strcmp(buffer, "exit\n") == 0) {
            printf("\nServer disconnected. Exiting chat...\n");
            pthread_cancel(send_thread);
            break;
        }
        printf("\nRecieved message: %s", buffer);
        printf("Message to send : ");
        fflush(stdout);  // Redisplay prompt
    }
    close(sockfd);
    return NULL;
}

void *send_messages(void *arg) {
    int sockfd = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("Message to send : ");
        fflush(stdout);
        fgets(buffer, BUFFER_SIZE, stdin);
        sendto(sockfd, &buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (strcmp(buffer, "exit\n") == 0) {
            printf("Closing chat...\n");
            pthread_cancel(recv_thread);
            break;
        }
    }
    close(sockfd);
    return NULL;
}

int setup_socket(char ip_addr[]) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }
    return sockfd;
}

int main() {
    int sockfd, counter = 0;
    char buffer[BUFFER_SIZE];
    sockfd = setup_socket("127.0.0.1"); // Client mode
    if (sockfd < 0) exit(EXIT_FAILURE);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    socklen_t addr_len = sizeof(server_addr);

    pthread_create(&recv_thread, NULL, receive_messages, &sockfd);
    pthread_create(&send_thread, NULL, send_messages, &sockfd);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);
    return 0;
}