#include "server.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    printf("Game server (turn model A) starting...\n");
    printf("Using Network Design Spec: %s\n", "/mnt/data/Network_design_specification.pdf");
    printf("Using Game Rule: %s\n", "/mnt/data/gamerule.txt");

    server_run(5555); // default port
    return 0;
}
