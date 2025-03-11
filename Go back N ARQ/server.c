#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/select.h>

#define PORT 8080
#define WIN_SIZE 4
#define MAX_SEQ 10
#define LOSS_PROB 20   // 20% chance of packet loss
#define TIMEOUT_SEC 2  // Retransmit after 2 sec timeout

typedef struct {
    int seq_no;
    char data[50];
} Packet;

// Function to setup the UDP socket
int setup_socket(struct sockaddr_in *addr) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(PORT);
    bind(sockfd, (struct sockaddr *)addr, sizeof(*addr));
    return sockfd;
}

int main() {
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int sockfd = setup_socket(&client), base = 0, next = 0, ack;
    char buf[10];
    struct timeval timeout = {TIMEOUT_SEC, 0};
    fd_set read_fds;

    // Wait for client connection
    recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &len);
    printf("[SERVER] Client connected!\n");

    srand(time(NULL));

    while (base < MAX_SEQ) {
        // Send packets within the window
        while (next < base + WIN_SIZE && next < MAX_SEQ) {
            if (rand() % 100 >= LOSS_PROB) {  // Simulate packet loss
                Packet pkt = {next, ""};
                sprintf(pkt.data, "Packet %d", next);
                sendto(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&client, len);
                printf("[SENT] %d\n", next);
            } else {
                printf("[DROPPED] %d\n", next);
            }
            next++;
        }

        // Wait for ACK with timeout
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        select(sockfd + 1, &read_fds, NULL, NULL, &timeout);

        if (FD_ISSET(sockfd, &read_fds)) {
            recvfrom(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)&client, &len);
            printf("[ACK] %d\n", ack);
            base = ack + 1;  // Move the window forward
        } else {
            printf("[TIMEOUT] Resending from %d\n", base);
            next = base;  // Retransmit from the last unacknowledged packet
        }
    }

    // Signal client that transmission is complete
    sendto(sockfd, &(int){-1}, sizeof(int), 0, (struct sockaddr *)&client, len);
    printf("[SERVER] Transmission complete.\n");
    close(sockfd);
    return 0;
}
