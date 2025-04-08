#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3001 // Must match server
#define BUFFER_SIZE 100

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { AF_INET, htons(PORT), inet_addr("127.0.0.1") };

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) return 1;
    printf("Connected to Factorial Server.\n");
    
    char buffer[BUFFER_SIZE];
    printf("Enter number for Factorial: ");
    memset(buffer, 0, BUFFER_SIZE);
    fgets(buffer, BUFFER_SIZE, stdin);

    send(sockfd, buffer, strlen(buffer), 0);

    memset(buffer, 0, BUFFER_SIZE);
    recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
    printf("Server response: %s\n", buffer);

    close(sockfd);
    return 0;
}