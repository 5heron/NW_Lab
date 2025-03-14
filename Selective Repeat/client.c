#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define PORT 8080
#define WINDOW_SIZE 4
#define TIMEOUT 3
#define MAX_SEQ 10

int setup_socket(struct sockaddr_in *addr) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct timeval timeout = {TIMEOUT, 0};
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    addr->sin_family = AF_INET;
    addr->sin_port = htons(PORT);
    addr->sin_addr.s_addr = INADDR_ANY;
    return sockfd;
}

int main() {
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    char buffer[1024];
    int base = 0, next_seq = 0;
    int acked[MAX_SEQ] = {0}; // Tracks received ACKs
    int sockfd = setup_socket(&server_addr);

    printf("Client Connected!\n");

    while (base < MAX_SEQ) {
        while (next_seq < base + WINDOW_SIZE && next_seq < MAX_SEQ) {
            if (!acked[next_seq]) {
                sprintf(buffer, "%d", next_seq);
                sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len);
                printf("[SENT] Packet %d\n", next_seq);
            }
            next_seq++;
        }

        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, &addr_len) > 0) {
            int ack = atoi(buffer);
            printf("[ACK RECEIVED] Packet %d\n", ack);
            acked[ack] = 1;
            int flag = 0;
            while (base != MAX_SEQ && acked[base]){
                flag = 1;
                base++;  // Move window when base packet is ACKed
            }
            if (flag == 1 && base != MAX_SEQ)
                printf("[BASE] moved to %d\n",base);
        } else {
            printf("[TIMEOUT] Resending unACKed packets...\n");
            next_seq = base;  // Retransmit only unacked packets
        }
    }

    close(sockfd);
    printf("[CLIENT] Transmission complete.\n");
}