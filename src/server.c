#define _POSIX_C_SOURCE 200112L
#include "server.h"
#include "client_handler.h"
#include "game.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_PENDING 10

static int listen_sock = -1;

void server_run(int port) {
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        perror("socket");
        exit(1);
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(listen_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(listen_sock, MAX_PENDING) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Server listening on 0.0.0.0:%d\n", port);

    // initialize game state
    game_init();

    while (1) {
        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);
        int csock = accept(listen_sock, (struct sockaddr*)&cli, &len);
        if (csock < 0) {
            perror("accept");
            continue;
        }
        printf("Client connected: %s:%d sock=%d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port), csock);
        // create handler
        client_handler_spawn(csock);
    }
}
