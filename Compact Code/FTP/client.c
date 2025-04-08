// client.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 1024

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { AF_INET, htons(PORT), inet_addr("127.0.0.1") };

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Connection failed.\n");
        return 1;
    }

    char filename[BUFFER_SIZE], filedata[BUFFER_SIZE];
    printf("Enter filename to request: ");
    scanf("%s", filename);

    // Send filename to server
    send(sockfd, filename, strlen(filename), 0);

    // Receive file data or error
    int bytes = recv(sockfd, filedata, BUFFER_SIZE, 0);
    filedata[bytes] = '\0';

    if (strcmp(filedata, "No such file") == 0) {
        printf("Server says: No such file\n");
    } else {
        printf("File contents:\n%s\n", filedata);
    }

    close(sockfd);
    return 0;
}