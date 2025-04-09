#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(){
	int n, bkt_size, out_rate, dropped, bkt_lvl = 0;
	printf("Enter the number of iterations :");
	scanf("%d",&n);
	int npkts[n];
	printf("Enter the number of packets to be sent in each iteration\n");
	for(int i = 0; i < n ; i++) scanf("%d",&npkts[i]);
	printf("Enter the Bucket size : ");
	scanf("%d",&bkt_size);
	printf("Enter the outgoing rate : ");
	scanf("%d",&out_rate);
	for(int i = 0; i < n; i++){
		printf("[INCOMING] : %d packets\n",npkts[i]); // (npkts + bkt_lvl) is number of packets we are trying to fit in the bucket.
		dropped = (npkts[i] + bkt_lvl) > bkt_size ? npkts[i] + bkt_lvl - bkt_size : 0;// (npkts + bkt_lvl) - bkt_size is the no of packets we couldnt fit (Dropped).  
		printf("[DROPPED]  : %d packets\n",dropped);
		bkt_lvl = (npkts[i] + bkt_lvl) - dropped;
		printf("[BUCKET]   : %d packets\n",bkt_lvl);
        bkt_lvl > out_rate ? printf("[OUTGOING] : %d packets\n",out_rate) : printf("[OUTGOING] : %d packets\n",bkt_lvl);
        bkt_lvl = bkt_lvl > out_rate ? bkt_lvl - out_rate : 0; 
		printf("[BUCKET]   : %d packets\n",bkt_lvl);
	}
	while(bkt_lvl > 0){
		bkt_lvl > out_rate ? printf("[OUTGOING] : %d packets\n",out_rate) : printf("[OUTGOING] : %d packets\n",bkt_lvl);
        bkt_lvl = bkt_lvl > out_rate ? bkt_lvl - out_rate : 0;
		printf("[BUCKET]   : %d packets\n",bkt_lvl);
	}
	printf("[BUCKET]   : Empty\n");
}