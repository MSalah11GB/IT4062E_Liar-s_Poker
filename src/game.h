#ifndef GAME_H
#define GAME_H

int game_add_player(int sock, const char *name);
void game_remove_player(int player_id);
void game_set_player_name(int player_id, const char *name);
int game_is_host(int player_id);
void game_start(void);
void game_init(void);

// client actions
void game_action_call(int player_id);
void game_action_fold(int player_id);
void game_action_allin(int player_id);
void game_action_switch(int player_id, int replace_index, int option_index);

#endif
