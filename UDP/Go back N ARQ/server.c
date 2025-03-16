#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define LOSS_PROB 5
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
    char buffer[1024] = {0};
    int expected_seq = 0;  // Next expected sequence number
    int last_ack_sent = -1;
    
    // Seed random generator
    srand(time(NULL));
    int sockfd = setup_socket(&server_addr);
    printf("[SERVER] Waiting for packets...\n");    
    while (1) {
        // Zero out buffer manually
        for (int i = 0; i < 1024; i++) {
            buffer[i] = 0;
        }
        // Receive packet
        recvfrom(sockfd, buffer, sizeof(buffer), 0, 
                (struct sockaddr *)&client_addr, &addr_len);
        int seq = atoi(buffer);
        printf("[RECEIVED] Packet %d\n", seq);
        // Simulate packet loss
        if (rand() % LOSS_PROB == 0) {
            printf("[DROPPED] Packet %d (simulating loss)\n", seq);
            continue;
        }
        // Go-Back-N: Accept only in-order packets
        if (seq == expected_seq) {
            printf("[ACCEPTED] Packet %d\n", seq);
            expected_seq++;
            last_ack_sent = seq;
        } else if (seq > expected_seq)
            printf("[DISCARDED] Out-of-order packet %d, expected %d\n", 
                  seq, expected_seq);
        else
            printf("[DUPLICATE] packet %d, expected %d\n", 
                  seq, expected_seq);
        // Simulate ACK loss
        if (rand() % LOSS_PROB == 0) {
            printf("[DROPPED] ACK %d (simulating ACK loss)\n", last_ack_sent);
            continue;
        }   
        // Send ACK
        sprintf(buffer, "%d", last_ack_sent);
        sendto(sockfd, buffer, strlen(buffer), 0, 
              (struct sockaddr *)&client_addr, addr_len);
        printf("[ACK] Sent %d\n", last_ack_sent);
        if (expected_seq == MAX_SEQ){
            printf("All packets received\n");
            break;
        }               
    }
    close(sockfd);
    return 0;
}
