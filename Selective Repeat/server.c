#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define LOSS_PROB 3  // 1 in 3 chance of loss
#define WINDOW_SIZE 4
#define MAX_SEQ 10

int setup_socket(struct sockaddr_in *addr) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(PORT);
    bind(sockfd, (struct sockaddr *)addr, sizeof(*addr));
    return sockfd;
}

int main() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024];
    int sockfd = setup_socket(&server_addr);

    int received[MAX_SEQ] = {0}; // Tracks received packets
    int expected_seq = 0;

    printf("[SERVER] Waiting for packets...\n");

    while (1) {
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
        int seq = atoi(buffer);
        printf("[RECEIVED] Packet %d\n", seq);

        if (rand() % LOSS_PROB == 0) {  
            printf("[DROPPED] Packet %d\n", seq);
            continue;
        }

        received[seq] = 1; // Mark packet as received

        while (received[expected_seq]) {
            expected_seq++;  // Deliver packets in order
        }

        if (rand() % LOSS_PROB == 0) {  
            printf("[DROPPED] ACK %d\n", seq);
            continue;
        }

        sprintf(buffer, "%d", seq);
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);
        printf("[ACK] %d\n", seq);
    }
}
