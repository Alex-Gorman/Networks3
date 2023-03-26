#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include <poll.h>
#include <math.h>

struct Router {
    int id;
    double cost;
    int nextHop;
    int initialized;
};

// struct 

int routerID;

/* Create the array of routers */
struct Router routers_array[20];

void parse_message(const char *message, int *id, int *cost, int *nextHop) {
    if (sscanf(message, "(%d,%d,%d)", id, cost, nextHop) != 3) {
        perror("Error parsing message\n");
        exit(1);
    }
}

void handleData(char* input) {
    // printf("got to handleData()\n");
    // printf("%s\n", input);

    for (int i = 0; i < 20; i++) {
        int id, cost, nextHop;
        parse_message(input, &id, &cost, &nextHop);
        printf("array_id=%d -> %d\n", routers_array[i].id, id);

        /* Check if the nextHop is the router we are on */
        if (nextHop == routerID) {
            // if (routers_array[i].id == id && routers_array[i].initialized == 1) break;
            if (routers_array[i].id == id) break;

            if (routers_array[i].initialized == 0) {
                routers_array[i].id = id;
                routers_array[i].cost = cost;
                routers_array[i].nextHop = nextHop;
                routers_array[i].initialized = 1;
                break;
            }
        }

        int getRouter = nextHop;
        double newCost = cost;
        
        if (routers_array[i].id == id) {
            double oldCost = routers_array[i].cost;

            while (getRouter != routerID) {
                for (int j = 0; j < 20; j++) {
                    if (routers_array[j].id == getRouter) {
                        newCost = newCost + routers_array[j].cost;
                        getRouter = routers_array[j].nextHop;
                    }
                }
            }

            if (newCost < oldCost) {
                routers_array[i].cost = newCost;
                routers_array[i].id = id;
                routers_array[i].initialized = 1;
                routers_array[i].nextHop = nextHop;
            }
            break;
        }


        // if (routers_array[i].id == id) break;
        if (routers_array[i].initialized != 0) continue;


        // int getRouter = nextHop;
        // double newCost = cost;
        /*  */
        while (getRouter != routerID) {
            for (int j = 0; j < 20; j++) {
                if (routers_array[j].id == getRouter) {
                    newCost = newCost + routers_array[j].cost;
                    getRouter = routers_array[j].nextHop;
                }
            }
        }

        routers_array[i].cost = newCost;
        routers_array[i].id = id;
        routers_array[i].initialized = 1;
        routers_array[i].nextHop = nextHop;
        break;
    }

}

void parse_string(char* input, char segments[][100], int* num_segments) {
    char* segment = strtok(input, "+");
    while (segment != NULL) {
        strcpy(segments[*num_segments], segment);
        (*num_segments)++;
        segment = strtok(NULL, "+");
    }
}

void print_table() {
    printf("Routing Table for router: %d\n", routerID);
    printf("Destination Router       Cost Route      NextHop\n");
    for (int i = 0; i < 20; i++) {
        if (routers_array[i].id != 0) printf("%-24d %-15.0f %d\n", routers_array[i].id, routers_array[i].cost, routers_array[i].nextHop);
    }
    printf("\n");
}

void reset_table() {
    for (int i = 0; i < 20; i++) {
        if (routers_array[i].initialized != 0) {
            routers_array[i].cost = 0;
            routers_array[i].nextHop = 0;
            routers_array[i].initialized = 0;
            routers_array[i].id = 0;
        }
    }
}

int main(int argc, char *argv[]) {
    int port = 30000;
    // int routerId;

    routerID = atoi(argv[1]);
    printf("routerID=%d\n", routerID);

    if (argc <=1) {
        perror("Please enter routerID\n");
        exit(1);
    }


    
    int server_socket;
    // int client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket error\n");
        exit(1);
    }

    /* Allow socket to bind to a port that is already in use */
    int reuse = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))<0) {
        perror("setsockopt(SO_REUSEADDR) failed\n");
        exit(1);
    }

    printf("Server Socket Created\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    // server_addr.sin_addr.s_addr = inet_addr("127.0.1.1");
    server_addr.sin_addr.s_addr = NULL;

    n = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (n < 0) {
        perror("bind error");
        exit(1);
    }
    printf("Socket binded to port number\n");

    listen(server_socket, 5);
    printf("Listening\n");

    /* Add the listening socket to the fd_set */
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(server_socket, &read_fds);
    int max_sd = server_socket;

    /* Creat Poll */
    struct pollfd mypoll;
    mypoll.fd = server_socket;
    mypoll.events = POLLIN;

    /* Create client fd array */
    // struct pollfd mypoll2;
    // mypoll2.fd = -1;
    // mypoll.events = POLLIN;

    /* Create an array of polls */
    struct pollfd pollArray[20];

    for (int i = 0; i < 20; i++) {
        pollArray[i].events = POLLIN;
        pollArray[i].fd = -1;
    }

    // /* Create the array of routers */
    // struct Router routers_array[20];

    while (1) {

        memset(buffer, 0, sizeof(buffer));

        fd_set temp_fds = read_fds;
        int temp_sd = max_sd;

        if (poll(&mypoll, 1, 100) == 1) {
            printf("got here 2\n");
            /* Accept the incoming client socket */
            int client_sock = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
            if (client_sock <= -1) {
                perror("Error creating client socket, please run again\n");
                exit(1);
            }
            printf("client socket = %d\n", client_sock);

            for (int i = 0; i < 20; i++) {
                if (pollArray[i].fd == -1) {
                    pollArray[i].fd = client_sock;
                    break;
                }
            }

            /* Add the incoming client socket to the set of fds */
            FD_SET(client_sock, &read_fds);
            FD_SET(client_sock, &temp_fds);
            if (client_sock > max_sd) {
                max_sd = client_sock;
                temp_sd = max_sd;
            }
        }

        printf("got here 11\n");

        for (int i = server_socket+1; i < temp_sd + 1; i++) {
            if (FD_ISSET(i, &temp_fds)) {
                printf("got here 12\n");

                for (int j = 0; j < 20; j++) {
                    if (pollArray[j].fd!=-1) {
                        printf("got here 13\n");
                        if (poll(&pollArray[j], 1, 100)) {
                            memset(buffer, 0, 1024);
                            printf("got here 14\n");
                            recv(pollArray[j].fd, buffer, sizeof(buffer), 0);
                            printf("buffer length = %ld\n", strlen(buffer));    
                            printf("buffer = %s\n", buffer);

                            /*  */
                            char segments[10][100];
                            int num_segments = 0;
                            parse_string(buffer, segments, &num_segments);

                            for (int i = 0; i < num_segments; i++) {
                                printf("Segment %d: %s\n", i + 1, segments[i]);
                                handleData(segments[i]);
                            }

                            // for (int i = 0; i < 20; i++) {

                            // }

                            // int id, cost, nextHop;
                            // parse_message(buffer, &id, &cost, &nextHop);
                            // printf("id=%d\n", id);
                            // printf("cost=%d\n", cost);
                            // printf("nextHop=%d\n", nextHop);


                        }
                    }
                }
            } else {
                printf("got here 13\n");
            }
        }

        /* Print table */
        print_table();
        
        /* Reset the table */
        reset_table();
        
        sleep(5);
    }

    return 0;
}

