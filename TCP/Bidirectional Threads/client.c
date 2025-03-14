#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100

pthread_t send_thread, recv_thread;

void *receive_messages(void *arg) {
    int sockfd = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(sockfd, buffer, BUFFER_SIZE, 0);
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
        send(sockfd, buffer, BUFFER_SIZE, 0);
        if (strcmp(buffer, "exit\n") == 0) {
            printf("Closing chat...\n");
            break;
        }
    }
    close(sockfd);
    return NULL;
}

int setup_socket(char ip_addr[]) {
    int sockfd;
    struct sockaddr_in addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(ip_addr);
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Connect failed\n");
        close(sockfd);
        return -1;
    }
    return sockfd;
}
int main() {
    int sockfd, counter = 0;
    char buffer[BUFFER_SIZE];
    sockfd = setup_socket("127.0.0.1"); // setup TCP client socket
    if (sockfd < 0) exit(EXIT_FAILURE);
    printf("Connected to server\n");
    pthread_create(&recv_thread, NULL, receive_messages, &sockfd);
    pthread_create(&send_thread, NULL, send_messages, &sockfd);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);

    return 0;

}