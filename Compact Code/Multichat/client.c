#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100

typedef struct {
    int client_id;
    char buffer[BUFFER_SIZE];
} packet;

void *receive_messages(void *arg) {
    packet *pkt = malloc(sizeof(packet));
    int bytes, sockfd = *(int *)arg;
    while ((bytes = recv(sockfd, pkt, sizeof(packet), 0)) > 0) {
        pkt->buffer[bytes - sizeof(pkt->client_id)] = '\0'; 
        printf("Message Received from %d : %s\n", pkt->client_id, pkt->buffer);
    }
    close(sockfd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { AF_INET, htons(PORT), inet_addr("127.0.0.1") };    
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) return 1;
    printf("Connected to server\n");

    packet *pkt = malloc(sizeof(packet));
    pkt->client_id = atoi(argv[1]);
    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_messages, &sockfd) != 0) exit(1);
    while (1) { 
        fgets(pkt->buffer, BUFFER_SIZE, stdin);  
        if (!(send(sockfd, pkt, sizeof(packet), 0) > 0)) break;
    }
    free(pkt);
    close(sockfd);
    return 0;
}