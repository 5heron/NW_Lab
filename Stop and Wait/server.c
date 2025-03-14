#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#define PORT 3003
#define MAX_SEQ 5

struct pkt {
	int ACK;
	int seqno;
	char data[25];
};

int setup_socket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Bind failed\n");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

void main()
{

    int sockfd, recvLen, drop, expected_seq = 0;
	struct pkt sendPkt, recvPkt;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    sockfd = setup_socket();
    printf("Server listening on port %d...\n", PORT);

	while(1)
	{
		recvLen = recvfrom(sockfd, &recvPkt, sizeof(recvPkt), 0, (struct sockaddr*)&client_addr, &addr_len);
		if (recvLen>=0)
        {
            drop = rand()%8;
            if (drop<=1){
                if (recvPkt.seqno == expected_seq % 2)
                    printf("\n[RECEIVED] Packet %d, but will be dropped to simulate [LOST PACKET]\n", recvPkt.seqno);
                else
                    printf("\n[DUPLICATE] Packet with no. %d discarded, sending ACK again\n", recvPkt.seqno);
                continue;
            }
            if (drop<=2){
                if (recvPkt.seqno == expected_seq % 2){
                    expected_seq++;
                    printf("\n[RECEIVED] Packet %d, but no ACK will be sent to simulate [LOST ACK]\n", recvPkt.seqno);
                }
                else
                    printf("\n[DUPLICATE] Packet with no. %d discarded, sending ACK again\n", recvPkt.seqno);
                continue;
            } 
            else{
                if (recvPkt.seqno == expected_seq % 2){
                    expected_seq++;
                    printf("\n[RECEIVED] Packet %d, sending ACK to simulate normal transmission\n", recvPkt.seqno);
                }
                else
                    printf("\n[DUPLICATE] Packet with no. %d discarded, sending ACK again\n", recvPkt.seqno);
                sendPkt.ACK =1;
                sendPkt.seqno = recvPkt.seqno;
                sendto(sockfd, &sendPkt, sizeof(sendPkt), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
                if (expected_seq == MAX_SEQ){
                    printf("All packets received\n");
                    break;
                }
            }
        }
        else
            printf("\nReceive Error\n");		
	}	
}