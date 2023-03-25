#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct Router {
    int id;
    int cost;
    int nextHop;
    int connected;
};

int main(int argc, char *argv[]) {

    /* check to see if user entered correct amount of arguments */
    if (argc <= 1) {
        printf("Error, please enter router ID(s)\n");
        exit(1);
    }

    int routerID;
    routerID = atoi(argv[1]);

    struct Router routers_array[20];

    for (int i = 2, j = 0; i < argc; i = i + 2, j++) {
        routers_array[j].id = atoi(argv[i]);
        routers_array[j].cost = atoi(argv[i+1]);
        routers_array[j].nextHop = -1;
        routers_array[j].connected = 0;
    }


    while (1) {




        /* Print table */
        printf("Routing Table for router: %d\n", routerID);
        printf("Destination Router       Cost Route      NextHop\n");
        for (int i = 0; i < 20; i++) {
            if (routers_array[i].id != 0) printf("%-24d %-15d %d\n", routers_array[i].id, routers_array[i].cost, routers_array[i].nextHop);
        }
        printf("\n");

        /* 2 second sleep, then run again */
        sleep(2);
    }




    return 1;
}