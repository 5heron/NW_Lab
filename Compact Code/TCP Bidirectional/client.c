#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { AF_INET, htons(PORT), inet_addr("127.0.0.1") };
    
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) return 1;
    printf("Connected to server\n");

    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sockfd, buffer, strlen(buffer), 0);
        if (!recv(sockfd, buffer, BUFFER_SIZE, 0)) break;
        printf("Server: %s", buffer);
    }
    close(sockfd);
}