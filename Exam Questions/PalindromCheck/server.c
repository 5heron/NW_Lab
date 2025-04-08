#include <stdio.h>
#include <string.h>
#include <stdbool.h> // For bool type (optional, could use int)
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3004
#define BUFFER_SIZE 100

// No separate helper function needed for this simple inline logic

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { AF_INET, htons(PORT), INADDR_ANY };

    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 1);
    printf("Palindrome Server (Reverse Method) listening on port %d...\n", PORT);

    int client_fd = accept(sockfd, NULL, NULL);
    printf("Client connected for palindrome check.\n");

    char buffer[BUFFER_SIZE];
    char reversed_buffer[BUFFER_SIZE]; // To store the reversed string
    char response[2*BUFFER_SIZE];
    bool is_palindrome = false;

    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    buffer[strcspn(buffer, "\r\n")] = 0; // Clean buffer

    // --- Simple String Reversal and Comparison ---
    int len = strlen(buffer);
    memset(reversed_buffer, 0, BUFFER_SIZE);
    for (int i = 0; i < len; i++) {
        reversed_buffer[i] = buffer[len - 1 - i];
    }
    reversed_buffer[len] = '\0'; // Null-terminate the reversed string

    // Compare original with reversed
    if (strcmp(buffer, reversed_buffer) == 0) {
        is_palindrome = true;
    }
    // --- End of Palindrome Check ---

    // Format the response
    sprintf(response, "\"%s\" is %s", buffer, is_palindrome ? "a Palindrome" : "Not a Palindrome");

    send(client_fd, response, strlen(response), 0);
    printf("Sent: %s\n", response);

    close(client_fd);
    close(sockfd);
    return 0;
}