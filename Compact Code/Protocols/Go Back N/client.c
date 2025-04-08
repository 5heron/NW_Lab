#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 3003
#define MAX_SEQ 10
#define WINDOW_SIZE 4
#define BUFFER_SIZE 25

struct pkt {
	int ACK;
	int seqno;
	char data[BUFFER_SIZE];
};

int main() {
    int sockfd, base = 0, recvLen;
    sockfd =  socket(AF_INET, SOCK_DGRAM, 0);
    
    struct timeval t ={2,0};
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&t, sizeof(t));
    struct sockaddr_in server_addr = { AF_INET, htons(PORT), inet_addr("127.0.0.1") };
    socklen_t addr_len = sizeof(server_addr);
    //connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)); //In TCP
    if (sockfd < 0) exit(EXIT_FAILURE);
    struct pkt sendPkt, recvPkt;
    sendPkt.seqno = 0, sendPkt.ACK = 1;

    while (base < MAX_SEQ) {
        while (sendPkt.seqno < base + WINDOW_SIZE && sendPkt.seqno < MAX_SEQ){
            //send(sockfd, &sendPkt, sizeof(sendPkt), 0); //In TCP
		    sendto(sockfd, &sendPkt, sizeof(sendPkt), 0, (struct sockaddr*)&server_addr, addr_len);
            printf("[SENT] Packet %d\n", sendPkt.seqno++);
        }
        //recvLen = recv(sockfd, &recvPkt, sizeof(recvPkt), 0); //In TCP
        recvLen = recvfrom(sockfd, &recvPkt, sizeof(recvPkt), 0, (struct sockaddr*)&server_addr, &addr_len);
        if (recvLen>0){
            printf("\nACK recieved for packet no. %d \n", recvPkt.seqno);
            if (recvPkt.seqno >= base) {
                base = recvPkt.seqno + 1;
                printf("[WINDOW] Updated base to %d\n", base);
            } 
        } else {
            printf("[TIMEOUT] Retransmitting all packets from %d to %d\n", 
                  base, sendPkt.seqno - 1);
            sendPkt.seqno = base;
        }
    }
    printf("All packets send\n");
}