#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100
#define BACKLOG 5  // Max pending connections

struct packet {
    int seq_no;
    int ack;
    char data[BUFFER_SIZE];
};

int setup_socket(int is_server, const char *ip_addr) {
    int sockfd;
    struct sockaddr_in addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    if (is_server) {
        addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            printf("Bind failed\n");
            close(sockfd);
            return -1;
        }
        if (listen(sockfd, BACKLOG) < 0) {
            printf("Listen failed\n");
            close(sockfd);
            return -1;
        }
    } else {
        addr.sin_addr.s_addr = inet_addr(ip_addr);
        if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            printf("Connect failed\n");
            close(sockfd);
            return -1;
        }
    }

    return sockfd;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    struct packet recv_pkt;

    server_fd = setup_socket(1, NULL);  // 1 = Server
    if (server_fd < 0) exit(EXIT_FAILURE);

    printf("Server listening on port %d...\n", PORT);

    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        printf("Client accept failed\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected\n");
    
    if (recv(client_fd, &recv_pkt, sizeof(recv_pkt), 0) > 0) {
        printf("Received packet:\n");
        printf("Sequence No: %d\n", recv_pkt.seq_no);
        printf("Data: %s\n", recv_pkt.data);
    } else {
        printf("Failed to receive data\n");
    }

    close(client_fd);
    close(server_fd);
    return 0;
}