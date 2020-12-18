#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "client.h"
#include "chat.h"

static Player *player = NULL;

void chat_version_print (void) {

	(void) printf (
		"\n\nChat %s - %s\n\n",
		CHAT_VERSION, CHAT_VERSION_DATE
	);

}

void chat_player_create (const char *name) {

	player = (Player *) malloc (sizeof (Player));
	if (player) {
		(void) strncpy (player->name, name, PLAYER_NAME_LEN);
	}

}