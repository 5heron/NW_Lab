#include <stdio.h>
#include <stdbool.h>

#define MAX_ROUTERS 100  // Maximum number of routers
#define INF 1000         // Represents infinity

// Router structure with fixed-size arrays
typedef struct {
    int distance[MAX_ROUTERS];    // Distance to each router
    int next_hop[MAX_ROUTERS];    // Next hop to reach each router
} Router;

int cost_matrix[MAX_ROUTERS][MAX_ROUTERS];  // Global cost matrix
Router routers[MAX_ROUTERS];                // Global router array

// Run distance vector algorithm to find shortest paths
void distance_vector_algorithm(int n) {
    bool updated;
    
    // Initialize distance and next_hop arrays
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            routers[i].distance[j] = (i == j) ? 0 : 
                (cost_matrix[i][j] != -1) ? cost_matrix[i][j] : INF;
            routers[i].next_hop[j] = (i == j || cost_matrix[i][j] != -1) ? j : -1;
        }
    }

    // Run algorithm until no updates
    do {
        updated = false;
        
        for (int i = 0; i < n; i++) {             // For each router
            for (int j = 0; j < n; j++) {         // For each destination
                if (i == j) continue;
                
                for (int k = 0; k < n; k++) {     // For each neighbor
                    if (cost_matrix[i][k] == -1) continue;
                    
                    int new_cost = cost_matrix[i][k] + routers[k].distance[j];
                    if (new_cost < routers[i].distance[j]) {
                        routers[i].distance[j] = new_cost;
                        routers[i].next_hop[j] = k;
                        updated = true;
                    }
                }
            }
        }
    } while (updated);
}

// Print the shortest path from start to destination
void print_path(int start, int dest, int n) {
    if (routers[start].distance[dest] == INF) {
        printf("No path exists from %d to %d\n", start, dest);
        return;
    }
    
    printf("Shortest path from %d to %d: ", start, dest);
    printf("Distance = %d\n", routers[start].distance[dest]);
    
    printf("Path: %d", start);
    int current = start;
    while (current != dest) {
        current = routers[current].next_hop[dest];
        printf(" -> %d", current);
    }
    printf("\n");
}

// Print routing tables for all routers
void print_all_routing_tables(int n) {
    for (int i = 0; i < n; i++) {
        printf("Routing table for router %d:\n", i);
        for (int j = 0; j < n; j++) {
            if (i == j) continue; // Skip self
            if (routers[i].distance[j] == INF) {
                printf("  Destination %d: unreachable\n", j);
            } else {
                printf("  Destination %d: next hop %d, distance %d\n", 
                    j, routers[i].next_hop[j], routers[i].distance[j]);
            }
        }
        printf("\n");
    }
}

int main() {
    int n; // Number of routers
    
    printf("Enter number of routers (max %d): ", MAX_ROUTERS);
    scanf("%d", &n);
    
    // Initialize cost matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cost_matrix[i][j] = (i == j) ? 0 : -1;
        }
    }
    
    // Input cost matrix
    printf("Enter the cost matrix (%d x %d):\n", n, n);
    printf("Use -1 for no direct connection, 0 for self-connection\n");
    
    for (int i = 0; i < n; i++) {
        printf("Enter costs from router %d to all others: ", i);
        for (int j = 0; j < n; j++) {
            scanf("%d", &cost_matrix[i][j]);
        }
    }
    
    // Run algorithm
    distance_vector_algorithm(n);
    
    // Print routing tables
    print_all_routing_tables(n);
    
    // Get start and destination
    int start, dest;
    printf("Enter start router: ");
    scanf("%d", &start);
    printf("Enter destination router: ");
    scanf("%d", &dest);
    
    // Print path
    print_path(start, dest, n);
    
    return 0;
}
