#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 3003
#define BUFFER_SIZE 25
#define MAX_SEQ 5

struct pkt {
	int ACK;
	int seqno;
	char data[BUFFER_SIZE];
};

int setup_socket() {
    struct timeval t ={2,0};
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&t, sizeof(t));
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }
    return sockfd;
}


void main()
{
    int sockfd, counter, recvLen;
	struct pkt sendPkt, recvPkt;
	struct sockaddr_in addr;
	socklen_t addrSize;
	sockfd = setup_socket();
	addrSize = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	while(counter != MAX_SEQ)
	{
		printf("\n[PACKET %d] Enter data: ",counter % 2);
		fgets(sendPkt.data, BUFFER_SIZE, stdin);
		sendPkt.ACK =0;
		sendPkt.seqno = counter % 2;
		sendto(sockfd, &sendPkt, sizeof(sendPkt), 0, (struct sockaddr*)&addr, addrSize);
		
		while(1)
		{
			recvLen = recvfrom(sockfd, &recvPkt, sizeof(recvPkt), 0, (struct sockaddr*)&addr, &addrSize);
			if (recvLen>=0){
				printf("\nACK recieved for packet no. %d \n", recvPkt.seqno);
                counter++;
                break;		
			}
			else{
				printf("\nTimeout! Pkt will be resent\n");
				sendto(sockfd, &sendPkt, sizeof(sendPkt), 0, (struct sockaddr*)&addr, addrSize);
			}		
		}	
	}
    printf("All packets send\n");
}

