#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 3003
#define MAX_SEQ 10

struct pkt {
	int ACK;
	int seqno;
	char data[25];
};

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0), drop, expected_seq = 0;
    struct sockaddr_in addr = { AF_INET, htons(PORT), INADDR_ANY };
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    struct pkt sendPkt, recvPkt;
    sendPkt.seqno = -1;
    
    if (sockfd < 0) exit(EXIT_FAILURE);
    printf("Server listening on port %d...\n", PORT);

    while (1) {
        if (!(recvfrom(sockfd, &recvPkt, sizeof(recvPkt), 0, (struct sockaddr*)&client_addr, &addr_len) > 0)) break;
        printf("\n[RECEIVED] Packet %d\n", recvPkt.seqno);
        drop = rand()%8;
        if (drop<=1){
            printf("\n[DROPPED] to simulate [LOST PACKET]\n");
            continue;
        }

        if (recvPkt.seqno == expected_seq){ 
            printf("[ACCEPTED] Packet %d\n", recvPkt.seqno);
            sendPkt.seqno = recvPkt.seqno;
        }
        else if (recvPkt.seqno < expected_seq) printf("\n[DUPLICATE] Packet %d discarded\n", recvPkt.seqno);
        else printf("[DISCARDED] Out-of-order packet %d, expected %d\n", recvPkt.seqno, expected_seq);

        if (drop<=2){
            if (recvPkt.seqno == expected_seq) expected_seq++;
            printf("[DROPPED] ACK %d (simulating ACK loss)\n", sendPkt.seqno);
            continue;
        }
        else{
            if (recvPkt.seqno == expected_seq) expected_seq++;
            printf("\n%d",expected_seq);
            printf("\n[ACK] Packet %d, sending ACK\n", sendPkt.seqno);
            sendPkt.ACK =1;
            sendto(sockfd, &sendPkt, sizeof(sendPkt), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
            if (expected_seq == MAX_SEQ) break;
        }
    }
    printf("All packets received\n");
    close(sockfd);
}