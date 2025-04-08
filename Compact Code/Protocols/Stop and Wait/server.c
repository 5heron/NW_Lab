#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 3003
#define MAX_SEQ 5

struct pkt {
	int ACK;
	int seqno;
	char data[25];
};

int main() {
    //In TCP use SOCK_STREAM 
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0), drop, expected_seq = 0;
    struct sockaddr_in addr = { AF_INET, htons(PORT), INADDR_ANY };
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    //Difference in TCP is highlighted in comments 
    //listen(sockfd, 5);
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    //int client_fd = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len);
    struct pkt sendPkt, recvPkt;
    
    if (sockfd < 0) exit(EXIT_FAILURE);
    printf("Server listening on port %d...\n", PORT);

    while (1) {
    //if (!(recv(client_fd, &recvPkt, sizeof(recvPkt), 0))) break; //In TCP
        if (!(recvfrom(sockfd, &recvPkt, sizeof(recvPkt), 0, (struct sockaddr*)&client_addr, &addr_len) > 0)) break;
        printf("\n[RECEIVED] Packet %d\n", recvPkt.seqno);
        drop = rand()%8;
        if (drop<=1){
            printf("\n[DROPPED] to simulate [LOST PACKET]\n");
            continue;
        }
        if (recvPkt.seqno != expected_seq % 2) printf("\n[DUPLICATE] Packet %d discarded\n", recvPkt.seqno);
        if (drop<=2){
            if (recvPkt.seqno == expected_seq % 2) expected_seq++;
            printf("\nNo ACK will be sent to simulate [LOST ACK]\n");
            continue;
        }
        else{
            if (recvPkt.seqno == expected_seq % 2) expected_seq++;
            printf("\n[ACK] Packet %d, sending ACK\n", recvPkt.seqno);
            sendPkt.ACK =1;
            sendPkt.seqno = recvPkt.seqno;
            //send(client_fd, &sendPkt, sizeof(sendPkt), 0); //In TCP
            sendto(sockfd, &sendPkt, sizeof(sendPkt), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
            if (expected_seq == MAX_SEQ) break;
        }
    }
    printf("All packets received\n");
    close(sockfd);
}