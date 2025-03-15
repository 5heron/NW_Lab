#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int setup_socket(char ip_addr[]) {
    int sockfd;
    struct sockaddr_in addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(ip_addr);
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Connect failed\n");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int main() {
    int sockfd = setup_socket("127.0.0.1");
    int packet = 0, response;
    
    while (1) {
        send(sockfd, &packet, sizeof(int), 0);
        printf("[CLIENT] Sent %d packets\n", packet);

        recv(sockfd, &response, sizeof(int), 0);
        if (response == -1) {
            printf("[CLIENT] Packet dropped! Retrying...\n");
            sleep(2);  // Wait before retrying
            send(sockfd, &packet, sizeof(int), 0);
        }
        packet++;
        sleep(rand() % 3 + 1);  // Random delay (1-3s)
    }

    close(sockfd);
    return 0;
}
