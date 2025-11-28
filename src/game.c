// core game logic: turn-based (model A), sequential turns, betting, showdown with poker evaluator
#define _POSIX_C_SOURCE 200112L
#include "game.h"
#include "poker.h"
#include "json_utils.h"
#include "util.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MAX_PLAYERS 8
#define NAME_MAX 64
#define ACTION_TIMEOUT_SEC 15

typedef enum {P_WAITING, P_IN_ROUND, P_FOLDED, P_SPECTATOR} player_state_t;

typedef struct {
    int sock;
    char name[NAME_MAX];
    int active; // 1 if slot used
    player_state_t state;
    int eliminated;
    int all_in;
    int reached_risk;
    char private_cards[2][3];
} player_t;

static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
static player_t players[MAX_PLAYERS];
static int player_count = 0;
static int host_id = 0;
static int game_running = 0;

// shared cards
static char shared_cards[5][3];
static int shared_count = 0;

// deck
static char deck[52][3];
static int deck_n = 0;

static void shuffle_deck() {
    const char *ranks = "23456789TJQKA";
    const char *suits = "CDHS";
    int idx=0;
    for(int r=0;r<13;r++) for(int s=0;s<4;s++){
        deck[idx][0] = ranks[r];
        deck[idx][1] = suits[s];
        deck[idx][2] = 0;
        idx++;
    }
    for(int i=51;i>0;i--) {
        int j = rand() % (i+1);
        char tmp[3]; memcpy(tmp, deck[i], 3); memcpy(deck[i], deck[j],3); memcpy(deck[j],tmp,3);
    }
    deck_n = 52;
}

int game_add_player(int sock, const char *name) {
    pthread_mutex_lock(&g_lock);
    if (player_count >= MAX_PLAYERS) { pthread_mutex_unlock(&g_lock); return -1; }
    int id = -1;
    for (int i=0;i<MAX_PLAYERS;i++){
        if (!players[i].active) { id = i; break; }
    }
    if (id < 0) { pthread_mutex_unlock(&g_lock); return -1; }
    players[id].active = 1;
    players[id].sock = sock;
    strncpy(players[id].name, name, sizeof(players[id].name)-1);
    players[id].state = P_WAITING;
    players[id].eliminated = 0;
    players[id].all_in = 0;
    players[id].reached_risk = 0;
    player_count++;
    if (player_count==1) host_id = id;
    pthread_mutex_unlock(&g_lock);
    return id;
}
void game_remove_player(int player_id) {
    pthread_mutex_lock(&g_lock);
    if (player_id>=0 && player_id<MAX_PLAYERS && players[player_id].active) {
        players[player_id].active = 0;
        player_count--;
    }
    pthread_mutex_unlock(&g_lock);
}
void game_set_player_name(int player_id, const char *name) {
    pthread_mutex_lock(&g_lock);
    if (player_id>=0 && player_id<MAX_PLAYERS && players[player_id].active) {
        strncpy(players[player_id].name, name, sizeof(players[player_id].name)-1);
    }
    pthread_mutex_unlock(&g_lock);
}
int game_is_host(int player_id) {
    pthread_mutex_lock(&g_lock);
    int res = (player_id==host_id);
    pthread_mutex_unlock(&g_lock);
    return res;
}

static void broadcast(const char *json) {
    pthread_mutex_lock(&g_lock);
    for(int i=0;i<MAX_PLAYERS;i++){
        if (players[i].active && !players[i].eliminated) {
            send_raw_json(players[i].sock, strdup(json));
        }
    }
    pthread_mutex_unlock(&g_lock);
}

// Internal helper: send state update
static void broadcast_state() {
    cJSON *arr = cJSON_CreateArray();
    pthread_mutex_lock(&g_lock);
    for(int i=0;i<MAX_PLAYERS;i++){
        if (players[i].active) {
            cJSON *p = cJSON_CreateObject();
            cJSON_AddStringToObject(p, "name", players[i].name);
            cJSON_AddStringToObject(p, "status", players[i].eliminated ? "spectator" : "alive");
            cJSON_AddItemToArray(arr, p);
        }
    }
    pthread_mutex_unlock(&g_lock);
    char *s = json_build_object("GAME_STATE_UPDATE", arr);
    broadcast(s);
    free(s);
}

static void deal_private_cards() {
    pthread_mutex_lock(&g_lock);
    for(int i=0;i<MAX_PLAYERS;i++){
        if (!players[i].active || players[i].eliminated) continue;
        // two cards
        strcpy(players[i].private_cards[0], deck[--deck_n]);
        strcpy(players[i].private_cards[1], deck[--deck_n]);
        // send private cards to player only
        cJSON *arr = cJSON_CreateArray();
        cJSON_AddItemToArray(arr, cJSON_CreateString(players[i].private_cards[0]));
        cJSON_AddItemToArray(arr, cJSON_CreateString(players[i].private_cards[1]));
        char *payload = json_build_object("GAME_PRIVATE_CARDS", arr);
        send_raw_json(players[i].sock, payload);
        free(payload);
    }
    pthread_mutex_unlock(&g_lock);
}

static void deal_shared_cards_count(int count) {
    for(int k=0;k<count;k++){
        strcpy(shared_cards[shared_count++], deck[--deck_n]);
    }
    // build json array
    cJSON *arr = cJSON_CreateArray();
    for(int i=0;i<shared_count;i++) cJSON_AddItemToArray(arr, cJSON_CreateString(shared_cards[i]));
    char *payload = json_build_object("GAME_SHARED_UPDATE", arr);
    broadcast(payload);
    free(payload);
}

// Player actions pushed from client handlers
// For sequential-turn model, these simply set flags checked by the game loop
static int pending_action_player = -1;
static pthread_mutex_t action_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t action_cond = PTHREAD_COND_INITIALIZER;
static char action_type[32];

void game_action_call(int player_id) {
    pthread_mutex_lock(&action_lock);
    if (pending_action_player == player_id) {
        strcpy(action_type, "CALL");
        pthread_cond_signal(&action_cond);
    } else {
        // ignore or warn
    }
    pthread_mutex_unlock(&action_lock);
}
void game_action_fold(int player_id) {
    pthread_mutex_lock(&action_lock);
    if (pending_action_player == player_id) {
        strcpy(action_type, "FOLD");
        pthread_cond_signal(&action_cond);
    }
    pthread_mutex_unlock(&action_lock);
}
void game_action_allin(int player_id) {
    pthread_mutex_lock(&action_lock);
    if (pending_action_player == player_id) {
        strcpy(action_type, "ALL_IN");
        pthread_cond_signal(&action_cond);
    }
    pthread_mutex_unlock(&action_lock);
}
void game_action_switch(int player_id, int replace_index, int option_index) {
    // For simplicity, map option_index 0..2 to top-of-deck picks in this server design
    pthread_mutex_lock(&g_lock);
    if (player_id>=0 && player_id<MAX_PLAYERS && players[player_id].active && !players[player_id].eliminated) {
        // replace private_cards[replace_index] with a card drawn from deck according to option_index (we'll pop from deck)
        if (replace_index < 0) replace_index = 0;
        if (replace_index > 1) replace_index = 1;
        // apply option_index as skip count into deck (bounded)
        int idx = deck_n - 1 - (option_index % 3);
        if (idx < 0) idx = deck_n - 1;
        if (idx >=0) {
            strcpy(players[player_id].private_cards[replace_index], deck[idx]);
            // remove that card from deck by shifting last card into idx
            memcpy(deck[idx], deck[--deck_n], 3);
        }
        // inform all
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "player_id", player_id);
        cJSON_AddStringToObject(obj, "player", players[player_id].name);
        cJSON_AddNumberToObject(obj, "replaced_index", replace_index);
        cJSON_AddStringToObject(obj, "new_card", players[player_id].private_cards[replace_index]);
        char *payload = json_build_object("GAME_SWITCH_RESULT", obj);
        broadcast(payload);
        free(payload);
    }
    pthread_mutex_unlock(&g_lock);
}

// Core round logic (single-threaded)
static void *game_thread(void *arg) {
    srand(time(NULL));
    // main game loop
    while (1) {
        // wait until at least 2 active non-eliminated players and game_running == 1
        pthread_mutex_lock(&g_lock);
        int ready = 0;
        int alive = 0;
        for(int i=0;i<MAX_PLAYERS;i++){
            if (players[i].active && !players[i].eliminated) alive++;
        }
        ready = (alive >= 2 && game_running);
        pthread_mutex_unlock(&g_lock);
        if (!ready) {
            sleep(1);
            continue;
        }

        // prepare deck and deal
        shared_count = 0;
        shuffle_deck();
        deal_private_cards();

        // Switch offer: build 3 options (top 3 of deck)
        char options[3][3];
        for(int i=0;i<3;i++){ strcpy(options[i], deck[--deck_n]); }
        // broadcast options
        cJSON *opts = cJSON_CreateArray();
        for(int i=0;i<3;i++) cJSON_AddItemToArray(opts, cJSON_CreateString(options[i]));
        char *offer = json_build_object("GAME_SWITCH_OFFER", opts);
        broadcast(offer);
        free(offer);

        // Wait for each player to switch sequentially (force at least one replacement)
        pthread_mutex_lock(&g_lock);
        for(int i=0;i<MAX_PLAYERS;i++){
            if (!players[i].active || players[i].eliminated) continue;
            // for now we simulate a required switch: if player does not reply, auto replace index 0 with options[rand]
            // In a future improvement we'll collect explicit GAME_SWITCH messages.
            int replaced_idx = 0;
            int picked_opt = rand()%3;
            strcpy(players[i].private_cards[replaced_idx], options[picked_opt]);
            // broadcast result
            cJSON *res = cJSON_CreateObject();
            cJSON_AddStringToObject(res, "player", players[i].name);
            cJSON_AddNumberToObject(res, "replaced_index", replaced_idx);
            cJSON_AddStringToObject(res, "new_card", players[i].private_cards[replaced_idx]);
            char *res_s = json_build_object("GAME_SWITCH_RESULT", res);
            broadcast(res_s);
            free(res_s);
        }
        pthread_mutex_unlock(&g_lock);

        // Betting rounds with strict sequential turns
        // Risk levels as per game rule: 1..5 (we iterate five stages)
        int RISK_LEVELS[] = {1,2,3,4,5};
        for (int stage=0; stage<5; stage++) {
            int risk = RISK_LEVELS[stage];
            // First betting round (stage==0) => disallow ALL_IN
            int allow_all_in = (stage != 0);

            // Broadcast risk level
            cJSON *rj = cJSON_CreateObject();
            cJSON_AddNumberToObject(rj, "risk", risk);
            char *riskmsg = json_build_object("GAME_RISK_LEVEL_UPDATE", rj);
            broadcast(riskmsg);
            free(riskmsg);

            // reveal shared cards at proper stages:
            if (stage == 2) { // flop (choose 3)
                deal_shared_cards_count(3);
            } else if (stage == 3) { // turn +1
                deal_shared_cards_count(1);
            } else if (stage == 4) { // river +1
                deal_shared_cards_count(1);
            }

            // For each active (non-eliminated) player in seat order, ask to act (sequential)
            pthread_mutex_lock(&g_lock);
            int seat_order[MAX_PLAYERS]; int seats = 0;
            for (int i=0;i<MAX_PLAYERS;i++){
                if (players[i].active && !players[i].eliminated) {
                    players[i].state = P_IN_ROUND;
                    players[i].reached_risk = 0;
                    players[i].all_in = 0;
                    seat_order[seats++] = i;
                }
            }
            pthread_mutex_unlock(&g_lock);

            for (int s=0;s<seats;s++){
                int pid = seat_order[s];
                // if player already folded earlier in this round, skip (we use state P_FOLDED)
                pthread_mutex_lock(&g_lock);
                if (players[pid].state == P_FOLDED || players[pid].eliminated) { pthread_mutex_unlock(&g_lock); continue; }
                pthread_mutex_unlock(&g_lock);

                // send TURN message to player
                cJSON *turn = cJSON_CreateObject();
                cJSON_AddStringToObject(turn, "player", players[pid].name);
                cJSON_AddNumberToObject(turn, "risk", risk);
                char *turn_msg = json_build_object("GAME_TURN", turn);
                // send to all so clients know whose turn it is
                broadcast(turn_msg);
                free(turn_msg);

                // set pending action
                pthread_mutex_lock(&action_lock);
                pending_action_player = pid;
                action_type[0] = 0;
                // wait up to ACTION_TIMEOUT_SEC for an action
                struct timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_sec += ACTION_TIMEOUT_SEC;
                int rc = 0;
                while (action_type[0] == 0 && rc == 0) {
                    rc = pthread_cond_timedwait(&action_cond, &action_lock, &ts);
                }
                // copy action
                char local_action[32] = {0};
                if (action_type[0] != 0) strncpy(local_action, action_type, sizeof(local_action)-1);
                pthread_mutex_unlock(&action_lock);

                // apply default on timeout
                if (local_action[0] == 0) {
                    // auto-CALL on timeout
                    strcpy(local_action, "CALL");
                    // broadcast auto action
                    cJSON *autoobj = cJSON_CreateObject();
                    cJSON_AddStringToObject(autoobj, "player", players[pid].name);
                    cJSON_AddStringToObject(autoobj, "action", "AUTO_CALL");
                    char *automs = json_build_object("GAME_ACTION_BROADCAST", autoobj);
                    broadcast(automs);
                    free(automs);
                } else {
                    // broadcast player's action
                    cJSON *obj = cJSON_CreateObject();
                    cJSON_AddStringToObject(obj, "player", players[pid].name);
                    cJSON_AddStringToObject(obj, "action", local_action);
                    char *actmsg = json_build_object("GAME_ACTION_BROADCAST", obj);
                    broadcast(actmsg);
                    free(actmsg);
                }

                // apply action
                pthread_mutex_lock(&g_lock);
                if (strcmp(local_action, "FOLD")==0) {
                    players[pid].state = P_FOLDED;
                } else if (strcmp(local_action, "ALL_IN")==0) {
                    if (!allow_all_in) {
                        // invalid; treat as CALL instead
                        players[pid].all_in = 0;
                        players[pid].reached_risk = risk;
                    } else {
                        players[pid].all_in = 1;
                        players[pid].reached_risk = risk;
                    }
                } else { // CALL or fallback
                    players[pid].reached_risk = risk;
                }
                pthread_mutex_unlock(&g_lock);

                // check for round-ending conditions (if only one remains)
                pthread_mutex_lock(&g_lock);
                int active_remaining = 0;
                int last_idx = -1;
                for (int i=0;i<MAX_PLAYERS;i++){
                    if (players[i].active && !players[i].eliminated && players[i].state != P_FOLDED) {
                        active_remaining++;
                        last_idx = i;
                    }
                }
                pthread_mutex_unlock(&g_lock);
                if (active_remaining <= 1) {
                    // broadcast immediate winner
                    if (active_remaining == 1) {
                        cJSON *w = cJSON_CreateObject();
                        cJSON_AddStringToObject(w, "winner", players[last_idx].name);
                        char *wm = json_build_object("GAME_ROUND_END", w);
                        broadcast(wm);
                        free(wm);
                    }
                    break;
                }
            } // end seat loop

            // proceed to next stage
        } // end stages

        // Showdown: evaluate hands for non-folded players
        pthread_mutex_lock(&g_lock);
        // build list of contenders
        int contenders[8], ncont=0;
        for (int i=0;i<MAX_PLAYERS;i++){
            if (players[i].active && !players[i].eliminated && players[i].state != P_FOLDED) {
                contenders[ncont++] = i;
            }
        }
        if (ncont == 0) {
            // weird: everyone folded -> continue next round
            pthread_mutex_unlock(&g_lock);
            continue;
        }
        // evaluate best hand for each contender
        int best_score = -1;
        int worst_score = 1000000000;
        int scores[8];
        for (int i=0;i<ncont;i++){
            int pid = contenders[i];
            // build card list: private 2 + shared_count
            char cards[7][3];
            int nc = 0;
            strcpy(cards[nc++], players[pid].private_cards[0]);
            strcpy(cards[nc++], players[pid].private_cards[1]);
            for (int k=0;k<shared_count;k++) strcpy(cards[nc++], shared_cards[k]);
            // evaluate
            hand_rank_t hr = poker_best_hand(cards, nc);
            int score = hr.score;
            scores[pid] = score;
            if (score > best_score) best_score = score;
            if (score < worst_score) worst_score = score;
        }

        // losers = those with score == worst_score
        cJSON *losers_arr = cJSON_CreateArray();
        for (int i=0;i<ncont;i++){
            int pid = contenders[i];
            if (scores[pid] == worst_score) {
                cJSON *o = cJSON_CreateObject();
                cJSON_AddStringToObject(o, "player", players[pid].name);
                cJSON_AddNumberToObject(o, "risk", players[pid].reached_risk);
                cJSON_AddItemToArray(losers_arr, o);
            }
        }
        char *showdown_msg = json_build_object_with_child("GAME_SHOWDOWN", "losers", losers_arr);
        broadcast(showdown_msg);
        free(showdown_msg);

        // Roulette resolution: simple deterministic random per bullet probability
        // Using same param as earlier: BULLET_HIT_PROB = 0.12
        for (int i=0;i<ncont;i++){
            int pid = contenders[i];
            if (scores[pid] == worst_score) {
                int bullets = players[pid].reached_risk;
                double per = 0.12;
                double survive = pow(1.0 - per, bullets);
                double r = (double)rand() / RAND_MAX;
                int eliminated = (r > survive) ? 1 : 0;
                if (eliminated) {
                    players[pid].eliminated = 1;
                    // inform
                    cJSON *obj = cJSON_CreateObject();
                    cJSON_AddStringToObject(obj, "player", players[pid].name);
                    cJSON_AddBoolToObject(obj, "survived", 0);
                    cJSON_AddStringToObject(obj, "reason", "bullet_hit");
                    char *rr = json_build_object("GAME_RISK_RESOLVE", obj);
                    broadcast(rr);
                    free(rr);
                } else {
                    cJSON *obj = cJSON_CreateObject();
                    cJSON_AddStringToObject(obj, "player", players[pid].name);
                    cJSON_AddBoolToObject(obj, "survived", 1);
                    cJSON_AddStringToObject(obj, "reason", "safe");
                    char *rr = json_build_object("GAME_RISK_RESOLVE", obj);
                    broadcast(rr);
                    free(rr);
                }
            }
        }

        // Reset per-player states for next round
        pthread_mutex_lock(&g_lock);
        for (int i=0;i<MAX_PLAYERS;i++){
            players[i].state = P_WAITING;
            players[i].reached_risk = 0;
            players[i].all_in = 0;
        }
        pthread_mutex_unlock(&g_lock);

        broadcast_state();

        // small delay
        sleep(2);
    } // end main loop

    return NULL;
}

void game_start(void) {
    pthread_mutex_lock(&g_lock);
    if (!game_running) {
        game_running = 1;
        // spawn game thread
        pthread_t tid;
        pthread_create(&tid, NULL, game_thread, NULL);
        pthread_detach(tid);
    }
    pthread_mutex_unlock(&g_lock);
}

void game_init(void) {
    srand(time(NULL));
    memset(players, 0, sizeof(players));
    player_count = 0;
    game_running = 0;
}
