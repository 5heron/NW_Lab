#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100

struct packet {
    int seq_no;
    int ack;
    char data[BUFFER_SIZE];
};

int setup_socket(int is_server, const char *ip_addr) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    
    if (is_server) {
        addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            printf("Bind failed\n");
            close(sockfd);
            return -1;
        }
    } else {
        addr.sin_addr.s_addr = inet_addr(ip_addr);
    }

    return sockfd;
}

int main() {
    int sockfd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    struct packet recv_pkt;

    sockfd = setup_socket(1, NULL);  // Server mode
    if (sockfd < 0) exit(EXIT_FAILURE);

    printf("Server listening on port %d...\n", PORT);

    if (recvfrom(sockfd, &recv_pkt, sizeof(recv_pkt), 0, (struct sockaddr*)&client_addr, &addr_len) > 0) {
        printf("Received packet:\n");
        printf("Sequence No: %d\n", recv_pkt.seq_no);
        printf("Data: %s\n", recv_pkt.data);
    } else {
        printf("Failed to receive data\n");
    }

    close(sockfd);
    return 0;
}