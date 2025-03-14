#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 1024

typedef struct {
    int client_id;
    char buffer[BUFFER_SIZE];
} packet;

void *receive_messages(void *arg) {
    packet *pkt = malloc(sizeof(packet));
    int sockfd = *(int *)arg;
    int bytes;
    while ((bytes = recv(sockfd, pkt, sizeof(packet), 0)) > 0) {
        pkt->buffer[bytes - sizeof(pkt->client_id)] = '\0'; 
        printf("Message Received from %d : %s\n", pkt->client_id, pkt->buffer);
    }
    if (bytes == 0)
        printf("Server disconnected\n");
    else if (bytes == -1)
        printf("Receive failed");
    close(sockfd);
    pthread_exit(NULL);
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <client_id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    packet *pkt = malloc(sizeof(packet));
    pkt->client_id = atoi(argv[1]);
    pthread_t receive_thread;
    int sockfd = setup_socket("127.0.0.1"); // setup TCP client socket
    if (sockfd < 0) exit(EXIT_FAILURE);
    printf("Connected to the server\n");

    if (pthread_create(&receive_thread, NULL, receive_messages, &sockfd) != 0) {
        printf("Thread creation failed");
        exit(1);
    }

    while (1) { 
        fgets(pkt->buffer, BUFFER_SIZE, stdin);  
        if (send(sockfd, pkt, sizeof(packet), 0) == -1) {
            printf("Send failed");
            break;
        }
    }
    free(pkt);
    close(sockfd);
    return 0;
}