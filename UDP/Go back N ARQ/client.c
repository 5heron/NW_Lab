#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define WINDOW_SIZE 4
#define TIMEOUT 1       // Timeout in seconds
#define MAX_PACKETS 10  

int setup_socket(struct sockaddr_in *addr) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    // Set timeout for recvfrom
    struct timeval timeout = {TIMEOUT, 0};
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    // Set up server address
    addr->sin_family = AF_INET;
    addr->sin_port = htons(PORT);    
    return sockfd;
}
int main() {
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    char buffer[1024] = {0};
    int base = 0;         // First unacknowledged packet
    int next_seq = 0;     // Next sequence number to send
    int sockfd = setup_socket(&server_addr);
    printf("[CLIENT] Started, sending up to %d packets...\n", MAX_PACKETS);
    
    while (base < MAX_PACKETS) {
        // Send packets within window
        while (next_seq < base + WINDOW_SIZE && next_seq < MAX_PACKETS) {
            // Send packet
            sprintf(buffer, "%d", next_seq);
            sendto(sockfd, buffer, strlen(buffer), 0, 
                  (struct sockaddr *)&server_addr, addr_len);
            printf("[SENT] Packet %d\n", next_seq);
            next_seq++;
        }
        
        // Zero out buffer manually
        for (int i = 0; i < 1024; i++) {
            buffer[i] = 0;
        }
        
        // Try to receive ACK
        int recv_result = recvfrom(sockfd, buffer, sizeof(buffer), 0, 
                                  (struct sockaddr *)&server_addr, &addr_len);
        
        if (recv_result > 0) {
            int ack = atoi(buffer);
            printf("[ACK RECEIVED] %d\n", ack);
            
            // Update base (cumulative ACK in Go-Back-N)
            if (ack >= base) {
                base = ack + 1;
                printf("[WINDOW] Updated base to %d\n", base);
            } 
        } else {
            // Timeout occurred
            printf("[TIMEOUT] Retransmitting all packets from %d to %d\n", 
                  base, next_seq - 1);
            
            // Go back to first unacknowledged packet
            next_seq = base;
        }
    }    
    close(sockfd);
    printf("[CLIENT] Transmission complete. Sent %d packets.\n", MAX_PACKETS);
    return 0;
}