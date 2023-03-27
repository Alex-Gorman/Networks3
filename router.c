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
#include <arpa/inet.h>

/* Struct to hold values for each routers values in table */
struct Router {
    int id;
    int cost;
    int nextHop;
    int initialized;
};

/* Struct to hold router and cost given by user */
struct Router_And_Cost {
    int routerID;
    int cost;
    int initialized;
    int sock;
};

/* Array to hold values of the routers to connect with and cost of these connections given by user */
struct Router_And_Cost router_and_cost_array[20];

/* Router ID given by user for this router */
int routerID;

/* Router ID array to hold all router ID's seen */
int routersID_array[20];

/* Create the array of routers */
struct Router routers_array[20];
struct Router router_and_cost_array_all_values[200];

/* Parse one part of router message do get the router id, cost and nextHop values */
void parse_message(const char *message, int *id, int *cost, int *nextHop) {
    if (sscanf(message, "(%d,%d,%d)", id, cost, nextHop) != 3) {
        perror("Error parsing message\n");
        exit(1);
    }
}

void handleData(char* input) {
    // printf("got to handleData()\n");
    // printf("input in handle data = %s\n", input);

    int id, cost, nextHop;
    parse_message(input, &id, &cost, &nextHop);

    /* Add to array of ID's seen */
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

    for (int i = 0; i < 200; i++) {
        if (router_and_cost_array_all_values[i].initialized != 1) {
            router_and_cost_array_all_values[i].id = id;
            router_and_cost_array_all_values[i].cost = cost;
            router_and_cost_array_all_values[i].nextHop = nextHop;
            router_and_cost_array_all_values[i].initialized = 1;
            break;
        }
    }

    /* Building the actual table */
    for (int i = 0; i < 20; i++) {
        // int id, cost, nextHop;
        // parse_message(input, &id, &cost, &nextHop);
        // printf("array_id=%d -> %d\n", routers_array[i].id, id);

        /* Check if the nextHop is the router we are on */
        // if (nextHop == routerID) {
        //     // if (routers_array[i].id == id && routers_array[i].initialized == 1) break;
        //     if (routers_array[i].id == id) break;

        //     if (routers_array[i].initialized == 0) {
        //         routers_array[i].id = id;
        //         routers_array[i].cost = cost;
        //         routers_array[i].nextHop = nextHop;
        //         routers_array[i].initialized = 1;
        //         break;
        //     }
        // }

        /* If nexthop is routerID then dont need to calculate total cost */
        if (nextHop == routerID) {
            if (routers_array[i].initialized != 0) continue;
            else {
                routers_array[i].id = id;
                routers_array[i].cost = cost;
                routers_array[i].nextHop = nextHop;
                routers_array[i].initialized = 1;
            }
        }

        // int getRouter = nextHop;
        // int newCost = cost;
        
        // if (routers_array[i].id == id) {
        //     int oldCost = routers_array[i].cost;

        //     while (getRouter != routerID) {
        //         for (int j = 0; j < 20; j++) {
        //             if (routers_array[j].id == getRouter) {
        //                 newCost = newCost + routers_array[j].cost;
        //                 getRouter = routers_array[j].nextHop;
        //             }
        //         }
        //     }

        //     if (newCost < oldCost) {
        //         if (newCost == 13) printf("YOLO 1\n");
        //         routers_array[i].cost = newCost;
        //         routers_array[i].id = id;
        //         routers_array[i].initialized = 1;
        //         routers_array[i].nextHop = nextHop;
        //     }
        //     break;
        // }

        int getRouter = nextHop;
        int newCost = cost;
        
        if (routers_array[i].id == id) {
            int oldCost = routers_array[i].cost;

            while (getRouter != routerID) {
                for (int j = 0; j < 200; j++) {
                    if (router_and_cost_array_all_values[j].id == getRouter) {
                        newCost = newCost + router_and_cost_array_all_values[j].cost;
                        getRouter = router_and_cost_array_all_values[j].nextHop;
                    }
                }
            }

            if (newCost < oldCost) {
                // if (newCost == 13) printf("YOLO 1\n");
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
        newCost = cost;
        while (getRouter != routerID) {
            for (int j = 0; j < 200; j++) {
                if (router_and_cost_array_all_values[j].id == getRouter) {
                    newCost = newCost + router_and_cost_array_all_values[j].cost;
                    getRouter = router_and_cost_array_all_values[j].nextHop;
                }
            }
        }

        // if (newCost == 13) printf("YOLO 2\n");
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

/* Reset all the values in the table */
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

// /* Create the message of all the current values in the table (routerID, cost, nextHop) */
// char* create_message() {
//     char* message = (char*)malloc(1024*sizeof(char));
//     // printf("message=%s\n", message);

//     /* Go through each router in the router array and add the values to the message to send */
//     for (int i = 0; i < 20; i++) {
//         if (routers_array[i].initialized != 0) {
            
//             char msg[10];
//             strcat(message, "(");
//             sprintf(msg, "%d", routers_array[i].id);
//             strcat(message, msg);
//             strcat(message, ",");
            
//             memset(msg, 0, sizeof(msg));
//             sprintf(msg, "%d", routers_array[i].cost);
//             strcat(message, msg);

//             strcat(message, ",");

//             memset(msg, 0, sizeof(msg));
//             sprintf(msg, "%d", routers_array[i].nextHop);

//             strcat(message, msg);
//             strcat(message, ")");

//             strcat(message, "+");
//         }
//     }
//     message[strlen(message)-1] = '\0';
//     // printf("message=%s\n", message);
//     return message;
// }


// /*  */
// char* create_broadcast_message(int router_id, int router_cost, int router_nextHop) {
//     char* msg = create_message();
//     // printf("msg=%s\n", msg);

//     char* message = (char*)malloc(1024*sizeof(char));

//     char temp[10];
//     strcat(message, "(");
//     sprintf(temp, "%d", router_id);
//     strcat(message, temp);
//     strcat(message, ",");
//     memset(temp, 0, sizeof(temp));
//     sprintf(temp, "%d", router_cost);
//     strcat(message, temp);
//     strcat(message, ",");
//     memset(temp, 0, sizeof(temp));
//     sprintf(temp, "%d", router_nextHop);
//     strcat(message, temp);
//     strcat(message, ")");

//     // printf("strlen og msg%ld\n", strlen(msg));

//     if (strlen(msg)<=1) return message;
//     else {
//         strcat(message, "+");
//         strcat(message, msg);
//         return message;
//         // printf("string=%s\n", message);
//     }

// }


/* Try to make a TCP connection with another router given the port number */
int make_tcp_connection_with_another_router(int portNum) {
    // int port = portNum;
    // printf("portNumTryingToConnectWith%d\n", portNum);
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
    // addr.sin_addr.s_addr = NULL;
    addr.sin_addr.s_addr = inet_addr("127.0.1.1");

    int cnct = connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    // printf("connect val =%d\n", cnct);

    if (cnct < 0) {
        return cnct;
    } else return sock;
}

int main(int argc, char *argv[]) {

    /* Get the router given by the user */
    routerID = atoi(argv[1]);

    /* If at very least no routerID is given then exit program */
    if (argc <=1) {
        perror("Please enter routerID\n");
        exit(1);
    }

    /* If you have been given the routerID and and multiple of pairs of destID and cost then argc divided by 2 should equal 0 */
    if (argc % 2 != 0) {
        perror("Please enter router(s) and cost value(s)\n");
        exit(1);
    }


    /* Go through each pair of destID and cost entered by user and add to the router and cost array */
    int index = 2;
    for (int i = 0; index < argc; i++) {
        router_and_cost_array[i].routerID = atoi(argv[index]);
        router_and_cost_array[i].cost = atoi(argv[index+1]);
        router_and_cost_array[i].initialized = 1;
        router_and_cost_array[i].sock = -1;
        index += 2;
    }


    /* Server socket for the router */
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    /* Create the server socket for router */
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


    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = routerID;
    server_addr.sin_addr.s_addr = inet_addr("127.0.1.1");
    // server_addr.sin_addr.s_addr = NULL;

    n = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (n < 0) {
        perror("bind error");
        exit(1);
    }

    listen(server_socket, 5);

    /* Add the listening socket to the fd_set */
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(server_socket, &read_fds);
    int max_sd = server_socket;

    /* Creat Poll */
    struct pollfd mypoll;
    mypoll.fd = server_socket;
    mypoll.events = POLLIN;

    /* Create an array of polls */
    struct pollfd pollArray[20];

    for (int i = 0; i < 20; i++) {
        pollArray[i].events = POLLIN;
        pollArray[i].fd = -1;
    }

    while (1) {

        memset(buffer, 0, sizeof(buffer));

        char send_string[1024] = "";

        fd_set temp_fds = read_fds;
        int temp_sd = max_sd;

        if (poll(&mypoll, 1, 100) == 1) {
            // printf("got here 2\n");
            /* Accept the incoming client socket */
            int client_sock = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
            if (client_sock <= -1) {
                perror("Error creating client socket, please run again\n");
                exit(1);
            }
            // printf("client socket = %d\n", client_sock);

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

        // printf("got here 11\n");

        for (int i = server_socket+1; i < temp_sd + 1; i++) {
            if (FD_ISSET(i, &temp_fds)) {
                // printf("got here 12\n");

                for (int j = 0; j < 20; j++) {
                    if (pollArray[j].fd!=-1) {
                        // printf("got here 13\n");
                        if (poll(&pollArray[j], 1, 100)) {
                            memset(buffer, 0, 1024);
                            // printf("got here 14\n");
                            recv(pollArray[j].fd, buffer, sizeof(buffer), 0);
                            // printf("buffer length = %ld\n", strlen(buffer));    
                            // printf("buffer = %s\n", buffer);

                            strcat(send_string, buffer);
                            strcat(send_string, "+");


                            /*  */
                            char segments[10][100];
                            int num_segments = 0;
                            // strcat(send_string, buffer);
                            parse_string(buffer, segments, &num_segments);

                            for (int i = 0; i < num_segments; i++) {
                                // printf("Segment %d: %s\n", i + 1, segments[i]);
                                handleData(segments[i]);
                            }
                        }
                    }
                }
            } 
        }

        // printf("send_string = %s\n", send_string);
        send_string[strlen(send_string)-1] = '\0';
        // printf("send_string = %s\n", send_string);

        /* Print table */
        print_table();

        /* Try to make connection to other routers */
        for (int i = 0; i < 20; i++) {

            if (router_and_cost_array[i].initialized == 1) {
                // char send_string[1024];
                // strcat(send_string, buffer);
                // printf("send_string = %s\n", buffer);
                // char* table_values = create_broadcast_message(routerID, router_and_cost_array[i].cost, router_and_cost_array[i].routerID);
                // printf("char table =%s\n", table_values);

                char send_new_val[1024] = "";
                /*  */

                char temp[10];
                strcat(send_new_val, "(");
                sprintf(temp, "%d", routerID);
                strcat(send_new_val, temp);
                strcat(send_new_val, ",");
                memset(temp, 0, sizeof(temp));
                sprintf(temp, "%d", router_and_cost_array[i].cost);
                strcat(send_new_val, temp);
                strcat(send_new_val, ",");
                memset(temp, 0, sizeof(temp));
                sprintf(temp, "%d", router_and_cost_array[i].routerID);
                strcat(send_new_val, temp);
                strcat(send_new_val, ")+");
                // strcat(send_new_val, send_string);

                if (strlen(send_string) == 0) {
                    send_new_val[strlen(send_new_val)-1] = '\0';
                } else {
                    strcat(send_new_val, send_string);
                }

                // strcat(send_new_val, send_string);
                // printf("send_new_Val = %s\n", send_new_val);



                /*  */

                if (router_and_cost_array[i].sock == -1) {
                    int connect_val = make_tcp_connection_with_another_router(router_and_cost_array[i].routerID);
                    if (connect_val < 0) continue;
                    else {
                        router_and_cost_array[i].sock = connect_val;
                        if (send(router_and_cost_array[i].sock, send_new_val, strlen(send_new_val), MSG_NOSIGNAL) < 0) {
                            router_and_cost_array[i].sock = -1;
                        }
                    }
                }
                else {
                    if (send(router_and_cost_array[i].sock, send_new_val, strlen(send_new_val), MSG_NOSIGNAL) < 0) {
                            router_and_cost_array[i].sock = -1;
                    }
                }
            }
        }
        reset_table();
        
        sleep(3);
    }
    return 0;
}

