#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUCKET_SIZE 10   // Maximum packets bucket can hold
#define LEAK_RATE 1      // Number of packets processed per second
#define PORT 8080
#define BACKLOG 5  // Max pending connections


int bucket = 0;  // Current bucket size

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

// Function to leak packets at a constant rate
void *leaky_bucket(void *arg) {
    while (1) {
        sleep(1);
        if (bucket > 0) {
            bucket -= LEAK_RATE;
            printf("[SERVER] Processed 1 packet. Remaining in bucket: %d\n", bucket);
        }
    }
}

// Function to handle incoming packets
void handle_client(int client_sock) {
    int packet, response;
    while (recv(client_sock, &packet, sizeof(int), 0) > 0) {
        if (bucket + packet <= BUCKET_SIZE) {
            bucket += packet;
            printf("[SERVER] Received %d packets. Bucket usage: %d/%d\n", packet, bucket, BUCKET_SIZE);
            response = 1;  // Successfully received
        } else {
            printf("[SERVER] Bucket overflow! %d packets dropped.\n", packet);
            response = -1;  // Notify client about packet drop
        }
        send(client_sock, &response, sizeof(int), 0);
    }
    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    server_sock = setup_socket();
    pthread_t leak_thread;
    pthread_create(&leak_thread, NULL, leaky_bucket, NULL);
    printf("[SERVER] Waiting for packets...\n");

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        handle_client(client_sock);
    }

    close(server_sock);
    return 0;
}