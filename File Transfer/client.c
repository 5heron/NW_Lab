#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

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

void receive_file(int sockfd, char *filename) {
    char buffer[BUFFER_SIZE];
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        printf("[CLIENT] Error opening file.\n");
        return;
    }

    while (recv(sockfd, buffer, BUFFER_SIZE, 0) > 0) {
        fprintf(fp, "%s", buffer);
        memset(buffer, 0, BUFFER_SIZE);
    }

    printf("[CLIENT] File received and saved as %s.\n", filename);
    fclose(fp);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char filename[BUFFER_SIZE];

    sockfd = setup_socket("127.0.0.1");   
    printf("[CLIENT] Connected to server.\n");

    printf("[CLIENT] Enter filename to request: ");
    fgets(filename, BUFFER_SIZE, stdin);

    send(sockfd, filename, strlen(filename), 0);
    receive_file(sockfd, filename);

    close(sockfd);
    return 0;
}
