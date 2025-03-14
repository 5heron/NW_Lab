#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100

int setup_socket(char ip_addr[]) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }
    return sockfd;
}

int main() {
    int sockfd, counter = 0;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    sockfd = setup_socket("127.0.0.1"); // Client mode
    if (sockfd < 0) exit(EXIT_FAILURE);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    socklen_t addr_len = sizeof(server_addr);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        printf("Enter message to send : ");
        fgets(buffer, BUFFER_SIZE, stdin);
        sendto(sockfd, &buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, addr_len);
        if (recvfrom(sockfd, &buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &addr_len) > 0)
            printf("Received message : %s", buffer);
        else {
            printf("Server disconnected.\n");
            break;
        }
    }
}