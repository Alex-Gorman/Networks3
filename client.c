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

    int cnct = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    printf("connect =%d\n", cnct);
    printf("Connected to server\n");

    while (1) {
        bzero(buffer, 1024);
        strcpy(buffer, "(30100,1,30000)+(30200,2,30100)");

        if (send(sock, buffer, strlen(buffer), MSG_NOSIGNAL) < 0) {
            break;
            perror("send failed\n");
        }

        sleep(5);
    }

    while (1) {
        printf("got here 1\n");
    }
}
