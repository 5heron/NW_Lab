#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 3003
#define BUFFER_SIZE 100
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
    int sockfd, counter = 0;
    char buffer[BUFFER_SIZE];
    sockfd = setup_socket("127.0.0.1");
    if (sockfd < 0) exit(EXIT_FAILURE);
    printf("Connected to server\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        printf("Enter message to send : ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sockfd, buffer, BUFFER_SIZE, 0);
        if (recv(sockfd, buffer, BUFFER_SIZE, 0) > 0) {
            printf("Received message : %s", buffer);
        } else {
            printf("Server disconnected.\n");
            break;
        }
    }
    close(sockfd);
}