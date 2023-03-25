#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include <poll.h>

int main() {
    int port = 30000;
    
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket error\n");
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
    struct pollfd mypoll2;
    mypoll2.fd = -1;
    mypoll.events = POLLIN;


    while (1) {

        memset(buffer, 0, sizeof(buffer));

        fd_set temp_fds = read_fds;
        int temp_sd = max_sd;

        if (poll(&mypoll, 1, 100) == 1) {
            printf("got here 2\n");
            /* Accept the incoming client socket */
            int client_sock = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
            printf("client socket = %d\n", client_sock);

            mypoll2.fd = client_sock;

            /* Add the incoming client socket to the set of fds */
            FD_SET(client_sock, &read_fds);
            if (client_sock > max_sd) {
                max_sd = client_sock;
            }
        }

        int valread;

        printf("got here 11\n");

        for (int i = server_socket+1; i < temp_sd + 1; i++) {
            if (FD_ISSET(i, &temp_fds)) {
                // printf("got here 12\n");
                // recv(i, buffer, sizeof(buffer), 0);
                // printf("buffer = %s\n");

                if (poll(&mypoll2, 1, 100) == 1) {
                    printf("got here 12\n");
                    recv(i, buffer, sizeof(buffer), 0);
                    printf("buffer = %s\n", buffer);
                    mypoll2.revents = 0;
                    // mypoll2.events = POLLIN;

                }

                // if (valread = read(i, buffer, 1024) == 0) {
                //     printf("got here 10\n");
                // }
            } else {
                // buffer[valread] = '\0';
                // printf("buffer = %s\n", buffer);
                printf("got here 13\n");
            }
        }

        printf("got here 3\n");

        // struct timeval tv;
        // tv.tv_sec = 2;

        // fd_set temp_fds = read_fds;
        // int activity = select(max_sd + 1, &temp_fds, NULL, NULL, &tv);
        // if (activity < 0) {
        //     printf("activity = %d\n", activity);
        // } else {
        //     printf("activity = %d\n", activity);
        // }

        // printf("max_sd=%d\n", max_sd);

        
        /* Handle incoming data */
        // for (int i = 0; i < max_sd + 1; i++) {
        //     if (FD_ISSET(i, &read_fds)) {
        //         if (i == server_socket) {
        //             int client_sock = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
        //         }
        //     }
        // }


        // addr_size = sizeof(client_addr);
        // printf("got here 1\n");
        // client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
        // printf("Client connected\n");

        // bzero(buffer, 1024);
        // recv(client_socket, buffer, sizeof(buffer), 0);
        // printf("From client: %s\n", buffer);


        sleep(10);
    }

    return 0;
}

// int main() {
//     int server_fd, new_socket, valread;
//     struct sockaddr_in address;
//     int opt = 1;
//     int addrlen = sizeof(address);
//     char buffer[1024] = {0};
//     char *hello = "Hello from server";

//     // Creating socket file descriptor
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//         perror("socket failed");
//         exit(EXIT_FAILURE);
//     }

//     // Forcefully attaching socket to the port 8080
//     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
//                                                   &opt, sizeof(opt))) {
//         perror("setsockopt");
//         exit(EXIT_FAILURE);
//     }
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(8080);

//     // Forcefully attaching socket to the port 8080
//     if (bind(server_fd, (struct sockaddr *)&address,
//                                  sizeof(address))<0) {
//         perror("bind failed");
//         exit(EXIT_FAILURE);
//     }

//     if (listen(server_fd, 3) < 0) {
//         perror("listen");
//         exit(EXIT_FAILURE);
//     }
    
//     if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
//                        (socklen_t*)&addrlen))<0) {
//         perror("accept");
//         exit(EXIT_FAILURE);
//     }

//     valread = read( new_socket , buffer, 1024);
//     printf("%s\n",buffer );
//     send(new_socket , hello , strlen(hello) , 0 );
//     printf("Hello message sent\n");

    
//     return 0;
// }
