#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100
#define MAX_CLIENTS 10


typedef struct {
    int sockfd;
    struct sockaddr_in address;
} client_t;

typedef struct {
    int client_id;
    char buffer[BUFFER_SIZE];
} packet;

client_t *clients[MAX_CLIENTS];


void broadcast_message(packet *pkt, int sender_socket) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i]->sockfd != sender_socket) {
            if (send(clients[i]->sockfd, pkt, sizeof(packet), 0) == -1)
                printf("Send failed\n");
        }
    }
}
void *receive_messages(void *arg) {
    client_t *cli = (client_t *)arg;
    packet *pkt = malloc(sizeof(packet));
    int bytes;
    while ((bytes = recv(cli->sockfd, pkt, sizeof(packet), 0)) > 0) {
        pkt->buffer[bytes - sizeof(pkt->client_id)] = '\0';
        printf("Received message from client %d : %s\n", pkt->client_id, pkt->buffer);
        broadcast_message(pkt, cli->sockfd);  
    }
    //Remove client from array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == cli) {
            clients[i] = NULL;
            break;
        }
    }
    close(cli->sockfd);
    free(cli);
    free(pkt);
    pthread_exit(NULL);
}

int main() {
    int client_sock, sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client_addr, addr = { AF_INET, htons(PORT), INADDR_ANY };
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;

    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 5);
    printf("Server started, waiting for clients to connect...\n");
    
    while ((client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &client_len)) > 0) {
        client_t *new_client = (client_t *)malloc(sizeof(client_t));
        new_client->sockfd = client_sock;
        new_client->address = client_addr;

        //Add the client to a free position in the array
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!clients[i]) {
                clients[i] = new_client;
                break;
            }
        }
        pthread_create(&thread_id, NULL, receive_messages, new_client);
    }
    close(sockfd);
    return 0;
}