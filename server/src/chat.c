#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "cerver.h"
#include "packets.h"
#include "chat.h"

#pragma region version

void chat_version_print (void) {

	(void) printf (
		"\n\nChat %s - %s\n\n",
		CHAT_VERSION, CHAT_VERSION_DATE
	);

}

#pragma endregion

#pragma region players

#define MAX_PLAYERS				16

static unsigned int n_players = 0;
static Player *players[MAX_PLAYERS];

static Player *player_new (void) {

	Player *player = (Player *) malloc (sizeof (Player));
	if (player) {
		player->client = NULL;
		(void) memset (player, 0, PLAYER_NAME_LEN);
	}

	return player;

}

static void player_delete (void *player_ptr) {

	if (player_ptr) {
		Player *player = (Player *) player_ptr;

		player->client = NULL;

		free (player_ptr);
	}

}

static void chat_player_register (Player *player) {

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (players[i] == NULL) {
			players[i] = player;
			n_players += 1;
			break;
		}
	}

}

static Player *chat_player_get_by_client (Client *client) {

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (players[i]) {
			if (players[i]->client->sock_fd == client->sock_fd) {
				return players[i];
			}
		}
	}

	return NULL;

}

void chat_player_create (const Client *client) {

	Player *player = player_new ();
	if (player) {
		player->client = client;

		chat_player_register (player);
	}

}

#pragma endregion

#pragma region main

unsigned int chat_init (void) {

	for (int i = 0; i < MAX_PLAYERS; i++) {
		players[i] = NULL;
	}

	return 0;

}

void chat_end (void) {

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (players[i]) player_delete (players[i]);
		players[i] = NULL;
	}

}

#pragma endregion

#pragma region handlers

void chat_on_name_received (
	Client *client,
	const char *name, const size_t name_size
) {

	(void) printf (
		"[%d]: %.*s\n",
		client->sock_fd, (int) name_size, name
	);

	Player *player = chat_player_get_by_client (client);
	if (player) {
		(void) strncpy (player->name, name, PLAYER_NAME_LEN);

		char text[1024] = { 0 };
		(void) snprintf (
			text, 1024,
			"Player %s has joined!",
			player->name
		);
		chat_cerver_broadcast_text_to_remaining_clients (
			client,
			text, strlen (text)
		);
	}

}

void chat_on_text_received (
	Client *client,
	const char *text, const size_t text_size
) {

	(void) printf (
		"[%d]: %.*s\n",
		client->sock_fd, (int) text_size, text
	);

	Player *player = chat_player_get_by_client (client);
	if (player) {
		char message[2048] = { 0 };
		(void) snprintf (
			message, 2048,
			"Player %s says: %s",
			player->name, text
		);

		chat_cerver_broadcast_text_to_remaining_clients (
			client,
			message, strlen (message)
		);
	}

}

#pragma endregion