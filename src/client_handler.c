// handles a single client: reads JSON messages, validates, and forwards actions to game module
#include "client_handler.h"
#include "json_utils.h"
#include "game.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    int sock;
    char name[64];
    int player_id; // index in game
} client_ctx_t;

static void *client_thread(void *arg) {
    client_ctx_t *ctx = (client_ctx_t*)arg;
    int sock = ctx->sock;
    char buf[4096];
    ssize_t r;
    // initial: register as player in game
    ctx->player_id = game_add_player(sock, ctx->name);
    if (ctx->player_id < 0) {
        // room full or error
        send_raw_json(sock, json_build_error("ROOM_FULL", "Room is full"));
        close(sock);
        free(ctx);
        return NULL;
    }

    // send welcome and lobby info
    send_raw_json(sock, json_build_simple("LOBBY_JOIN_RESULT", "{\"success\":true}"));

    while ((r = recv(sock, buf, sizeof(buf)-1, 0)) > 0) {
        buf[r] = 0;
        // handle multiple JSON lines
        char *line = strtok(buf, "\n");
        while (line) {
            cJSON *root = json_parse(line);
            if (!root) {
                send_raw_json(sock, json_build_error("INVALID_JSON","Could not parse"));
                line = strtok(NULL, "\n");
                continue;
            }
            cJSON *type = cJSON_GetObjectItem(root, "type");
            if (!type || type->type != cJSON_String) {
                send_raw_json(sock, json_build_error("MISSING_TYPE","Missing 'type' field"));
                cJSON_Delete(root);
                line = strtok(NULL, "\n");
                continue;
            }
            const char *t = type->valuestring;

            // basic auth/login handling
            if (strcmp(t, "AUTH_LOGIN") == 0) {
                cJSON *u = cJSON_GetObjectItem(root, "username");
                if (u && u->type == cJSON_String) {
                    strncpy(ctx->name, u->valuestring, sizeof(ctx->name)-1);
                    ctx->name[sizeof(ctx->name)-1] = 0;
                    // update game player name
                    game_set_player_name(ctx->player_id, ctx->name);
                    send_raw_json(sock, json_build_simple("AUTH_LOGIN_RESULT","{\"success\":true}"));
                } else {
                    send_raw_json(sock, json_build_error("INVALID_PAYLOAD","Missing username"));
                }
            } else if (strcmp(t, "LOBBY_START_GAME") == 0) {
                // Only allow first player to start (host)
                if (game_is_host(ctx->player_id)) {
                    send_raw_json(sock, json_build_simple("LOBBY_START_GAME_RESULT","{\"success\":true}"));
                    game_start();
                } else {
                    send_raw_json(sock, json_build_error("NOT_HOST","Only host can start"));
                }
            } else if (strcmp(t, "GAME_CALL") == 0) {
                game_action_call(ctx->player_id);
            } else if (strcmp(t, "GAME_FOLD") == 0) {
                game_action_fold(ctx->player_id);
            } else if (strcmp(t, "GAME_ALL_IN") == 0) {
                game_action_allin(ctx->player_id);
            } else if (strcmp(t, "GAME_SWITCH") == 0) {
                // expect replace_index and option_index
                cJSON *ri = cJSON_GetObjectItem(root, "replace_index");
                cJSON *oi = cJSON_GetObjectItem(root, "option_index");
                if (ri && oi && ri->type==cJSON_Number && oi->type==cJSON_Number) {
                    game_action_switch(ctx->player_id, ri->valueint, oi->valueint);
                } else {
                    send_raw_json(sock, json_build_error("INVALID_SWITCH","Need replace_index and option_index"));
                }
            } else if (strcmp(t, "SYSTEM_PING") == 0) {
                send_raw_json(sock, json_build_simple("SYSTEM_PONG","{}"));
            } else {
                send_raw_json(sock, json_build_error("UNKNOWN_TYPE","Unknown message type"));
            }

            cJSON_Delete(root);
            line = strtok(NULL, "\n");
        }
    }

    // connection closed
    game_remove_player(ctx->player_id);
    close(sock);
    free(ctx);
    return NULL;
}

void client_handler_spawn(int sock) {
    client_ctx_t *ctx = calloc(1, sizeof(*ctx));
    ctx->sock = sock;
    snprintf(ctx->name, sizeof(ctx->name), "player%d", sock % 1000);

    pthread_t tid;
    pthread_create(&tid, NULL, client_thread, ctx);
    pthread_detach(tid);
}
