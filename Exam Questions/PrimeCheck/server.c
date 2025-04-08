#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For atoi
#include <stdbool.h> // For bool
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100

// Prime Check Helper
bool isPrime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { AF_INET, htons(PORT), INADDR_ANY };

    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 1);
    printf("Prime Check Server listening on port %d...\n", PORT);

    int client_fd = accept(sockfd, NULL, NULL);
    printf("Client connected for prime check.\n");

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    buffer[strcspn(buffer, "\r\n")] = 0; // Clean buffer

    int input_num = atoi(buffer);
    bool result = isPrime(input_num);

    sprintf(response, "%d is %s", input_num, result ? "Prime" : "Not Prime");

    send(client_fd, response, strlen(response), 0);
    printf("Sent: %s\n", response);

    close(client_fd);
    close(sockfd);
    return 0;
}