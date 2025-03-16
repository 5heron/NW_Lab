#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    sockfd =  socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_addr = { AF_INET, htons(PORT), inet_addr("127.0.0.1") };
    socklen_t addr_len = sizeof(server_addr);
    if (sockfd < 0) exit(EXIT_FAILURE);
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        printf("Enter message to send : ");
        fgets(buffer, BUFFER_SIZE, stdin);
        sendto(sockfd, &buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, addr_len);
        if (!(recvfrom(sockfd, &buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &addr_len) > 0)) break;
        printf("Received message : %s", buffer);
    }
}