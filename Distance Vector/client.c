#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define INF 9999
#define MAX_ROUTERS 10
#define PORT 8080

int setup_socket(char ip_addr[]) {
    int sockfd;
    struct sockaddr_in addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(ip_addr);
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Connect failed\n");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

void bellman_ford(int graph[MAX_ROUTERS][MAX_ROUTERS], int src, int num_routers, int *dist) {
    for (int i = 0; i < num_routers; i++)
        dist[i] = INF;
    
    dist[src] = 0;

    for (int i = 0; i < num_routers - 1; i++) {
        for (int u = 0; u < num_routers; u++) {
            for (int v = 0; v < num_routers; v++) {
                if (graph[u][v] != INF && dist[u] + graph[u][v] < dist[v])
                    dist[v] = dist[u] + graph[u][v];
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <router_id>\n", argv[0]);
        return 1;
    }

    int router_id = atoi(argv[1]);  // Router ID from command line
    int num_routers, sockfd;
    struct sockaddr_in server_addr;

    // Connect to the server
    sockfd = setup_socket("127.0.0.1"); // setup TCP client socket
    if (sockfd < 0) exit(EXIT_FAILURE);
    // Receive number of routers from server
    recv(sockfd, &num_routers, sizeof(int), 0);
    printf("[CLIENT %d] Number of routers: %d\n", router_id, num_routers);

    int graph[MAX_ROUTERS][MAX_ROUTERS];
    int dist[MAX_ROUTERS];

    // Receive adjacency matrix from server
    recv(sockfd, graph, sizeof(graph), 0);
    
    // Compute shortest paths
    bellman_ford(graph, router_id, num_routers, dist);

    // Send computed distance vector to server
    send(sockfd, dist, num_routers * sizeof(int), 0);
    printf("[CLIENT %d] Sent distance vector to server.\n", router_id);

    close(sockfd);
    return 0;
}