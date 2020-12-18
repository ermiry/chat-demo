#ifndef _CHAT_H_
#define _CHAT_H_

#include <stdbool.h>

#define CHAT_VERSION				"0.1"
#define CHAT_VERSION_DATE			"17/12/2020"

extern void chat_version_print (void);

#define PLAYER_NAME_LEN			64

typedef struct Player {

	char name[PLAYER_NAME_LEN];

} Player;

extern void chat_player_create (const char *name);

extern void chat_player_display_cards (void);

extern int chat_player_send_card (int selected_card);

extern void chat_on_cards_received (const char *buffer);

extern void chat_on_game_started_received (void);

extern void chat_on_game_ended_received (void);

#endif