#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3003
#define BUFFER_SIZE 100

struct packet {
    int seq_no;
    int ack;
    char data[BUFFER_SIZE];
};

int setup_socket(int is_server, const char *ip_addr) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    
    if (is_server) {
        addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            printf("Bind failed\n");
            close(sockfd);
            return -1;
        }
    } else {
        addr.sin_addr.s_addr = inet_addr(ip_addr);
    }

    return sockfd;
}

int main() {
    int sockfd, counter = 0;
    struct sockaddr_in server_addr;
    struct packet send_pkt;

    sockfd = setup_socket(0, "127.0.0.1"); // Client mode
    if (sockfd < 0) exit(EXIT_FAILURE);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    send_pkt.seq_no = counter++;
    send_pkt.ack = 0;

    printf("Enter data to send: ");
    fgets(send_pkt.data, BUFFER_SIZE, stdin);

    sendto(sockfd, &send_pkt, sizeof(send_pkt), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Data sent to server\n");

    close(sockfd);
    return 0;
}
