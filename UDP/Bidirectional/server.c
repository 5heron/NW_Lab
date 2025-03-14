#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100

int setup_socket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Bind failed\n");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int main() {
    int sockfd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    sockfd = setup_socket();
    if (sockfd < 0) exit(EXIT_FAILURE);
    printf("Server listening on port %d...\n", PORT);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);        
        if (recvfrom(sockfd, &buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len) > 0){
            printf("Recieved message : %s", buffer);
            printf("Enter message to send : ");
            fgets(buffer, BUFFER_SIZE, stdin);
            sendto(sockfd, &buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
        } else {
            printf("Client disconnected.\n");
            break;
        }
    }
    close(sockfd);
    return 0;
}