#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 3003
#define MAX_SEQ 5
#define BUFFER_SIZE 25

struct pkt {
	int ACK;
	int seqno;
	char data[BUFFER_SIZE];
};

int main() {
    int sockfd, counter = 0, recvLen;
    sockfd =  socket(AF_INET, SOCK_DGRAM, 0);
    
    struct timeval t ={2,0};
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&t, sizeof(t));
    struct sockaddr_in server_addr = { AF_INET, htons(PORT), inet_addr("127.0.0.1") };
    socklen_t addr_len = sizeof(server_addr);
    //connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)); //In TCP    
	if (sockfd < 0) exit(EXIT_FAILURE);
    struct pkt sendPkt, recvPkt;

    while(counter != MAX_SEQ)
	{
		printf("\n[PACKET %d] Enter data: ",counter % 2);
		fgets(sendPkt.data, BUFFER_SIZE, stdin);
		sendPkt.ACK =0;
		sendPkt.seqno = counter % 2;
        //send(sockfd, &sendPkt, sizeof(sendPkt), 0); //In TCP
		sendto(sockfd, &sendPkt, sizeof(sendPkt), 0, (struct sockaddr*)&server_addr, addr_len);
		
		while(1)
		{
        	//recvLen = recv(sockfd, &recvPkt, sizeof(recvPkt), 0); //In TCP
			recvLen = recvfrom(sockfd, &recvPkt, sizeof(recvPkt), 0, (struct sockaddr*)&server_addr, &addr_len);
			if (recvLen>=0){
				printf("\nACK recieved for packet no. %d \n", recvPkt.seqno);
                counter++;
                break;		
			}
			else{
				printf("\nTimeout! Pkt will be resent\n");
        		//send(sockfd, &sendPkt, sizeof(sendPkt), 0); //In TCP
				sendto(sockfd, &sendPkt, sizeof(sendPkt), 0, (struct sockaddr*)&server_addr, addr_len);
			}		
		}	
	}
    printf("All packets send\n");
}