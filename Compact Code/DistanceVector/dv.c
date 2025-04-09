#include<stdio.h>
#include<stdlib.h>
#define MAX_ROUTERS 20
#define INF 1000
typedef struct{
    int next_hop[MAX_ROUTERS], distanceTo[MAX_ROUTERS];
} Router;

int cost_matrix[MAX_ROUTERS][MAX_ROUTERS];
Router routers[MAX_ROUTERS];
int main(){
    int n, start,dest;
    printf("Enter the number of routers: ");
    scanf("%d",&n);
    printf("Enter the cost matrix :\n");
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            scanf("%d",&cost_matrix[i][j]);
            if (cost_matrix[i][j] < 0) cost_matrix[i][j] = INF;
        }
    }
    for(int i = 0;i < n; i++){
        for(int j = 0; j < n;j++){
            if (i == j) continue;
            routers[i].next_hop[j] = j;
            routers[i].distanceTo[j] = cost_matrix[i][j];
        }
    }
    int updated;
    do{
        updated = 0;
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                if(i == j) continue;
                for(int k = 0; k < n; k++){
                    if (cost_matrix[i][k] == INF) continue;
                    int new_cost = cost_matrix[i][k] + routers[k].distanceTo[j];
                    if (new_cost < routers[i].distanceTo[j]){
                        routers[i].distanceTo[j] = new_cost;
                        routers[i].next_hop[j] = k;
                        updated = 1;
                    }
                }
            }
        }
    }while(updated);
    // --- skip this bit ---
    printf("\n================ Routing Tables ================\n");
    printf("+========+=============+============+=============+\n");
    printf("| Router | Destination |  Next Hop  |  Distance   |\n");
    printf("+========+=============+============+=============+\n");
    for(int i = 0; i < n; i++) {
        int is_first_entry_for_router = 1;
        for(int j = 0; j < n; j++) {
            if (i == j) continue; 
            if(!is_first_entry_for_router) printf("|--------+-------------+------------+-------------|\n");
            is_first_entry_for_router = 0;
            if (routers[i].distanceTo[j] >= INF) printf("|  %3d   |     %3d     |    None    |     INF     |\n",i , j);
            else printf("|  %3d   |     %3d     |   %4d     |    %4d     |\n",i , j, routers[i].next_hop[j], routers[i].distanceTo[j]);
        }
         if (n > 1 && i < n -1 ) {
              printf("+--------+-------------+------------+-------------+\n");
         }
    }
    printf("+========+=============+============+=============+\n");
    //Till here
    printf("Enter the start : ");
    scanf("%d",&start);
    printf("Enter the destination : ");
    scanf("%d",&dest);
    if(routers[start].distanceTo[dest] != INF){
        printf("Shortest path distance :  %d\n",routers[start].distanceTo[dest]);
        printf("Shortest Path : %d",start);
        int current = start;
        while(current != dest){
            current = routers[current].next_hop[dest];
            printf(" -> %d",current);
        }
        printf("\n");
    }
    else printf("No path from %d to %d\n",start,dest);
    return 0;
}