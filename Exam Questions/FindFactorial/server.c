#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For atoi
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3001
#define BUFFER_SIZE 100

// Factorial Helper
long long factorial(int n) {
    if (n < 0) return 0;
    long long res = 1;
    for (int i = 2; i <= n; i++) res *= i;
    return res;
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { AF_INET, htons(PORT), INADDR_ANY };

    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 1);
    printf("Factorial Server listening on port %d...\n", PORT);

    int client_fd = accept(sockfd, NULL, NULL);
    printf("Client connected for factorial.\n");

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

    int input_num = atoi(buffer);
    long long result = factorial(input_num);

    sprintf(response, "Factorial(%d) = %lld", input_num, result);

    send(client_fd, response, strlen(response), 0);
    printf("Sent: %s\n", response);

    close(client_fd);
    close(sockfd);
    return 0;
}