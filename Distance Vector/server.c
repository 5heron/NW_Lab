#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define INF 9999
#define MAX_ROUTERS 10
#define PORT 8080
#define BACKLOG 10  // Max pending connections

int cost_matrix[MAX_ROUTERS][MAX_ROUTERS];  // Stores the adjacency matrix
int num_routers = 0;  // Number of routers in the network

int setup_socket() {
    int sockfd;
    struct sockaddr_in addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Bind failed\n");
        close(sockfd);
        return -1;
    }
    if (listen(sockfd, BACKLOG) < 0) {
        printf("Listen failed\n");
        close(sockfd);
        return -1;
    }
    return sockfd;
}


void *handle_router(void *arg) {
    int sock = *(int *)arg;
    int routing_table[MAX_ROUTERS];

    // Receive distance vector from client (router)
    recv(sock, routing_table, num_routers * sizeof(int), 0);
    printf("[SERVER] Received distance vector from a router:\n");

    for (int i = 0; i < num_routers; i++)
        printf("Router %d Distance: %d\n", i, routing_table[i]);

    close(sock);
    return NULL;
}

void simulate_link_failure() {
    int r1, r2;
    printf("Enter the routers to disconnect (e.g., '0 1' to remove link R0 ↔ R1): ");
    scanf("%d %d", &r1, &r2);
    
    cost_matrix[r1][r2] = 9999;  // Mark as disconnected
    cost_matrix[r2][r1] = 9999;  // Since it's bidirectional
    printf("[SERVER] Link between R%d and R%d failed!\n", r1, r2);
}

void add_new_router() {
    int new_router = num_routers;
    num_routers++;  // Increase router count

    // Initialize distances for the new router
    printf("Enter costs from R%d to existing routers (9999 if no connection):\n", new_router);
    for (int i = 0; i < num_routers - 1; i++) {
        printf("Cost R%d → R%d: ", new_router, i);
        scanf("%d", &cost_matrix[new_router][i]);
        cost_matrix[i][new_router] = cost_matrix[new_router][i];  // Symmetric
    }

    // Set self-distance to 0
    cost_matrix[new_router][new_router] = 0;
    printf("[SERVER] Router R%d added to the network!\n", new_router);
}


int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;

    server_sock = setup_socket(); // setup TCP server socket
    if (server_sock < 0) exit(EXIT_FAILURE);

    printf("Enter the number of routers: ");
    scanf("%d", &num_routers);

    printf("Enter the cost matrix (%d x %d):\n", num_routers, num_routers);
    for (int i = 0; i < num_routers; i++)
        for (int j = 0; j < num_routers; j++)
            scanf("%d", &cost_matrix[i][j]);

    printf("[SERVER] Waiting for routers to connect...\n");
    int i = 0;
    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }
        printf("[SERVER] Router connected: %s\n", inet_ntoa(client_addr.sin_addr));

        // Send number of routers and cost matrix to the connected client
        send(client_sock, &num_routers, sizeof(int), 0);
        send(client_sock, cost_matrix, sizeof(cost_matrix), 0);
        pthread_create(&thread_id, NULL, handle_router, &client_sock);
        i++;
        while(i == num_routers) {
            char command[10];
            printf("Enter 'add' to add a new router, 'fail' to simulate link failure, or 'exit' to stop: ");
            scanf("%s", command);
            
            if (strcmp(command, "add") == 0){
                i++;
                add_new_router();
            }
            else if (strcmp(command, "fail") == 0) 
                simulate_link_failure();
            else if (strcmp(command, "exit") == 0)
                i = 0;
        }
    }
    close(server_sock);
    return 0;
}
