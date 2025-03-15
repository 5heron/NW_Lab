#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(){
	int n, bkt_size, out_rate, dropped, bkt_lvl = 0;
	printf("Enter the number of iterations :");
	scanf("%d",&n);
	int npkts[n];
	printf("Enter the number of packets to be sent in each iteration\n");
	for(int i = 0; i < n ; i++){
		printf("Iteration %d :",i+1);
		scanf("%d",&npkts[i]);
	}
	printf("Enter the Bucket size : ");
	scanf("%d",&bkt_size);
	printf("Enter the outgoing rate : ");
	scanf("%d",&out_rate);
	for(int i = 0; i < n; i++){
		printf("[INCOMING] %d packets\n",npkts[i]);
		dropped = (npkts[i] + bkt_lvl) - bkt_size;
		if (dropped > 0){
			 printf("[DROPPED] %d packets\n",dropped);
			 bkt_lvl = (npkts[i] + bkt_lvl) - dropped;
		}
		else
		 	bkt_lvl = (npkts[i] + bkt_lvl);
		printf("Current number of packets in bucket : %d\n",bkt_lvl);
		if (bkt_lvl - out_rate > 0){
			printf("[OUTGOING] %d packets\n",out_rate);
		 	bkt_lvl = bkt_lvl - out_rate;
		 }
		else{
			printf("[OUTGOING] %d packets\n",bkt_lvl);
		 	bkt_lvl = 0;
		 }
		printf("Number of packets left in the bucket: %d\n",bkt_lvl);
	}
	while(bkt_lvl > 0){
		if (bkt_lvl - out_rate > 0){
			printf("[OUTGOING] %d packets\n",out_rate);
		 	bkt_lvl = bkt_lvl - out_rate;
		 }
		else{
			printf("[OUTGOING] %d packets\n",bkt_lvl);
		 	bkt_lvl = 0;
		 }
		printf("Number of packets left in the bucket: %d\n",bkt_lvl);
	}
	printf("Bucket empty\n");
}
