#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define PORT 3003
#define BACKLOG 10  // Max pending connections

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
void *handle_client(void *arg) {
    client_t *cli = (client_t *)arg;
    int bytes;
    packet *pkt = malloc(sizeof(packet));

    while ((bytes = recv(cli->sockfd, pkt, sizeof(packet), 0)) > 0) {
        pkt->buffer[bytes - sizeof(pkt->client_id)] = '\0';
        printf("Received message from client %d : %s\n", pkt->client_id, pkt->buffer);
        broadcast_message(pkt, cli->sockfd);  
    }

    if (bytes == 0)
        printf("Client disconnected : %s\n", inet_ntoa(cli->address.sin_addr));
    else if (bytes == -1)
        printf("Receive failed");
    
    //Remove client
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

int setup_socket() {
    int sockfd, opt = 1;
    struct sockaddr_in addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); //To free port when client exit, allow port reuse.
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
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;
    server_sock = setup_socket(); // setup TCP server socket
    if (server_sock < 0) exit(EXIT_FAILURE);
    printf("Server started, waiting for clients to connect...\n");

    while ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) > 0) {
        if (client_sock == -1) {
            printf("Accept failed");
            continue;
        }
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

        if (pthread_create(&thread_id, NULL, handle_client, new_client) != 0)
            printf("Thread creation failed");
    }
    if (client_sock == -1) {
        printf("Accept failed");
        exit(1);
    }
    close(server_sock);
    return 0;
}