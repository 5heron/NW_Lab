#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define MAX_SEQ 10
#define LOSS_PROB 10  // 10% chance of ACK loss

typedef struct {
    int seq_no;
    char data[50];
} Packet;

int setup_socket(struct sockaddr_in *server) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = inet_addr("127.0.0.1");
    server->sin_port = htons(PORT);
    return sockfd;
}

int main() {
    struct sockaddr_in server;
    socklen_t len = sizeof(server);
    int sockfd = setup_socket(&server), expected = 0;
    Packet pkt;

    sendto(sockfd, "HELLO", 6, 0, (struct sockaddr *)&server, len);
    printf("\n[CLIENT] 🏁 Connected to server!\n");

    srand(time(NULL));

    while (1) {
        if (recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&server, &len) < 0) continue;
        if (pkt.seq_no == -1) break;  

        printf("[CLIENT] RECEIVED %s\n", pkt.data);

        if (pkt.seq_no == expected) {
            if (rand() % 100 >= LOSS_PROB) {
                sendto(sockfd, &expected, sizeof(expected), 0, (struct sockaddr *)&server, len);
                printf("[CLIENT] ACK Sent: %d\n", expected);
            } else {
                printf("[CLIENT] ACK LOST: %d\n", expected);
            }
            expected++;
        } else {
            printf("[CLIENT] OUT-OF-ORDER Packet %d (Expected %d)\n", pkt.seq_no, expected);
        }
    }

    printf("\n[CLIENT] All packets received. Exiting.\n");
    close(sockfd);
    return 0;
}
