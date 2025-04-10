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
    //In TCP use SOCK_STREAM    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0), drop, counter = 0, received[MAX_SEQ] = {0};
    struct sockaddr_in addr = { AF_INET, htons(PORT), INADDR_ANY };
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    //Difference in TCP is highlighted in comments 
    //listen(sockfd, 5);
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    //int client_fd = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len);
    struct pkt sendPkt, recvPkt;
    sendPkt.seqno = -1;
    sendPkt.ACK = 1;
    
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
        if(received[recvPkt.seqno]) printf("[DUPLICATE] Packet %d Discarded\n",recvPkt.seqno);
        else{
            printf("[ACCEPTED] Packet %d\n", recvPkt.seqno);
            received[recvPkt.seqno] = 1; // Mark packet as received
        } 
        if (drop<=2){
            printf("[DROPPED] ACK %d (simulating ACK loss)\n", recvPkt.seqno);
            continue;
        }
        sendPkt.seqno = recvPkt.seqno;
        printf("\n[ACK] Packet %d, sending ACK\n", sendPkt.seqno);
        //send(client_fd, &sendPkt, sizeof(sendPkt), 0); //In TCP
        sendto(sockfd, &sendPkt, sizeof(sendPkt), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
        counter++;
        if (counter == MAX_SEQ) break;
    }
    printf("All packets received\n");
    close(sockfd);
}