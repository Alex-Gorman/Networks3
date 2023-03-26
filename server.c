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
    int cost;
    int nextHop;
    int initialized;
};

struct Router_And_Cost {
    int routerID;
    int cost;
    int initialized;
    int sock;
};

struct Router_And_Cost router_and_cost_array[20];

int routerID;

int routersID_array[20];

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

    int id, cost, nextHop;
    parse_message(input, &id, &cost, &nextHop);

    int found  = 0;
    for (int i = 0; i < 20; i++) {
        if (routersID_array[i] == id) {
            found = 1;
            break;
        }
    }

    if (!found) {
        for (int i = 0; i < 20; i++) {
            if (routersID_array[i] == 0) {
                routersID_array[i] = id;
                break;
            }
        }
    }

    for (int i = 0; i < 20; i++) {
        // int id, cost, nextHop;
        // parse_message(input, &id, &cost, &nextHop);
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
        if (routers_array[i].id != 0) printf("%-24d %-15.0d %d\n", routers_array[i].id, routers_array[i].cost, routers_array[i].nextHop);
    }

    for (int i = 0; i < 20; i++) {
        if (routersID_array[i] != 0) {
            int found = 0;
            for (int j = 0; j < 20; j++) {
                if (routers_array[j].id == routersID_array[i]) {
                    found = 1;
                }
            }
            
            char* infinityString = "INF";

            // if (!found) printf("%-24d %-15.0s %s\n", routersID_array[i], "INF", "N/A");
            if (!found) printf("%-24d %s %15s\n", routersID_array[i], infinityString, "N/A");
        }
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

char* create_message() {
    char* message = (char*)malloc(1024*sizeof(char));
    // printf("message=%s\n", message);

    for (int i = 0; i < 20; i++) {
        if (routers_array[i].initialized != 0) {
            
            char msg[10];
            strcat(message, "(");
            sprintf(msg, "%d", routers_array[i].id);
            strcat(message, msg);
            strcat(message, ",");
            
            memset(msg, 0, sizeof(msg));
            sprintf(msg, "%d", routers_array[i].cost);
            strcat(message, msg);

            strcat(message, ",");

            memset(msg, 0, sizeof(msg));
            sprintf(msg, "%d", routers_array[i].nextHop);

            strcat(message, msg);
            strcat(message, ")");

            strcat(message, "+");
        }
    }
    message[strlen(message)-1] = '\0';
    // printf("message=%s\n", message);
    return message;
}

char* create_broadcast_message(int router_id, int router_cost, int router_nextHop) {
// void create_broadcast_message(int router_id, int router_cost, int router_nextHop) {
    char* msg = create_message();
    // printf("msg=%s\n", msg);

    char* message = (char*)malloc(1024*sizeof(char));

    char temp[10];
    strcat(message, "(");
    sprintf(temp, "%d", router_id);
    strcat(message, temp);
    strcat(message, ",");
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "%d", router_cost);
    strcat(message, temp);
    strcat(message, ",");
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "%d", router_nextHop);
    strcat(message, temp);
    strcat(message, ")");

    printf("strlen og msg%ld\n", strlen(msg));

    if (strlen(msg)<=1) return message;
    else {
        strcat(message, "+");
        strcat(message, msg);
        return message;
        // printf("string=%s\n", message);
    }

}


int make_tcp_connection_with_another_router(int portNum) {
    int port = portNum;
    printf("portNumTryingToConnectWith%d\n", portNum);
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket error\n");
        exit(1);
    }

    memset(&addr, '0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = portNum;
    addr.sin_addr.s_addr = NULL;

    int cnct = connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    printf("connect val =%d\n", cnct);

    if (cnct < 0) {
        return cnct;
    } else return sock;
}

int main(int argc, char *argv[]) {
    // int port = 30000;
    // int routerId;

    routerID = atoi(argv[1]);
    printf("routerID=%d\n", routerID);

    printf("argc = %d\n", argc);
    printf("argv[2]=%s\n", argv[2]);

    if (argc <=1) {
        perror("Please enter routerID\n");
        exit(1);
    }

    if (argc % 2 != 0) {
        perror("Please enter router(s) and cost value(s)\n");
        exit(1);
    }

    // printf("Got here 22\n");
    int index = 2;
    for (int i = 0; index < argc; i++) {
        router_and_cost_array[i].routerID = atoi(argv[index]);
        router_and_cost_array[i].cost = atoi(argv[index+1]);
        router_and_cost_array[i].initialized = 1;
        router_and_cost_array[i].sock = -1;
        index += 2;
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
    server_addr.sin_port = routerID;
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
                        }
                    }
                }
            } else {
                printf("got here 13\n");
            }
        }

        /* Print table */
        print_table();

        /* Try to make connection to other routers */
        // broadcast_message();
        // char* msg = create_message();
        // printf("message=%s\n", msg);

        // create_broadcast_message()

        for (int i = 0; i < 20; i++) {
            if (router_and_cost_array[i].initialized == 1) {
                // printf("data=%d\n", router_and_cost_array[i].routerID);
                char* table_values = create_broadcast_message(routerID, router_and_cost_array[i].cost, router_and_cost_array[i].routerID);
                // printf("in main msg=%s\n", table_values);
                // int connect_val = make_tcp_connection_with_another_router(router_and_cost_array[i].routerID);
                // printf("a=%d\n", connect_val);

                // if (connect_val < 0) {
                //     continue;
                // }
                // else {

                // }

                if (router_and_cost_array[i].sock == -1) {
                    int connect_val = make_tcp_connection_with_another_router(router_and_cost_array[i].routerID);
                    if (connect_val < 0) continue;
                    else {
                        router_and_cost_array[i].sock = connect_val;
                        if (send(router_and_cost_array[i].sock, table_values, strlen(table_values), MSG_NOSIGNAL) < 0) {
                            router_and_cost_array[i].sock = -1;
                        }
                    }
                }
                else {
                    if (send(router_and_cost_array[i].sock, table_values, strlen(table_values), MSG_NOSIGNAL) < 0) {
                            router_and_cost_array[i].sock = -1;
                    }
                }


                
            }

        }

        /* Test connecting to server */
        // int port = 34000;
        // int sock2;
        // struct sockaddr_in addr;

        // sock2 = socket(AF_INET, SOCK_STREAM, 0);
        // if (sock2 < 0) {
        //     perror("socket error\n");
        //     exit(1);
        // }
        // printf("Socket created\n");

        // memset(&addr, '0', sizeof(addr));
        // addr.sin_family = AF_INET;
        // addr.sin_port = port;
        // addr.sin_addr.s_addr = NULL;

        // int cnct = connect(sock2, (struct sockaddr*)&addr, sizeof(addr));
        // printf("connect =%d\n", cnct);

        /* Reset the table */
        reset_table();
        
        sleep(5);
    }

    return 0;
}

