#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// void parse_message(const char *message, int *id, int *cost, int *nextHop) {
//     if (sscanf(message, "(%d,%d,%d)", id, cost, nextHop) != 3) {
//         perror("Error parsing message\n");
//         exit(1);
//     }
// }

int main() {
    int port = 30000;
    int sock;
    struct sockaddr_in addr;
    // socklen_t addr_size;
    char buffer[1024];
    // int n;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket error\n");
        exit(1);
    }
    printf("Socket created\n");

    memset(&addr, '0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = NULL;

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    printf("Connected to server\n");

    while (1) {
        bzero(buffer, 1024);
        strcpy(buffer, "(30100,1,30000)+(30200,2,30100)");
        // strcpy(buffer, "(30100,1,30100)");
    

        int send_val = send(sock, buffer, strlen(buffer), 0);
        printf("send_val=%d\n", send_val);

        sleep(5);
    }

    // bzero(buffer, 1024);
    // strcpy(buffer, "Hello from client");
    
    // send(sock, buffer, strlen(buffer), 0);

    /* */
    // char *line = NULL;
    // size_t len = 0;
    // ssize_t read;
    // read = getline(&line, &len, stdin);
    /* */

}

// int main(int argc, char const *argv[]) {
//     int sock = 0, valread;
//     struct sockaddr_in serv_addr;
//     char *hello = "Hello from client";
//     char buffer[1024] = {0};
//     if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//         printf("\n Socket creation error \n");
//         return -1;
//     }

//     memset(&serv_addr, '0', sizeof(serv_addr));

//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_port = htons(8080);

//     // Convert IPv4 and IPv6 addresses from text to binary form
//     if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
//         printf("\nInvalid address/ Address not supported \n");
//         return -1;
//     }

//     if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
//         printf("\nConnection Failed \n");
//         return -1;
//     }
//     send(sock , hello , strlen(hello) , 0 );
//     printf("Hello message sent\n");
//     valread = read( sock , buffer, 1024);
//     printf("%s\n",buffer );
//     return 0;
// }
