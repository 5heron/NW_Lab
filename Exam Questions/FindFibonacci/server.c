#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For atoi
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3002
#define BUFFER_SIZE 100

// Fibonacci Helper
long long fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    long long a = 0, b = 1, c;
    for (int i = 2; i <= n; i++) { c = a + b; a = b; b = c; }
    return b;
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { AF_INET, htons(PORT), INADDR_ANY };
    // int opt = 1;
    // setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)); //To reuse address server otherwise server port has a wait time before reuse
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 1);
    printf("Fibonacci Server listening on port %d...\n", PORT);

    int client_fd = accept(sockfd, NULL, NULL);
    printf("Client connected for fibonacci.\n");

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    buffer[strcspn(buffer, "\r\n")] = 0; // Clean buffer

    int input_num = atoi(buffer);
    long long result = fibonacci(input_num);

    sprintf(response, "Fibonacci(%d) = %lld", input_num, result);

    send(client_fd, response, strlen(response), 0);
    printf("Sent: %s\n", response);

    close(client_fd);
    close(sockfd);
    return 0;
}