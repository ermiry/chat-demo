#ifndef _CHAT_H_
#define _CHAT_H_

#include <stdbool.h>
#include <stddef.h>

#pragma region version

#define CHAT_VERSION				"0.1"
#define CHAT_VERSION_DATE			"21/11/2020"

extern void chat_version_print (void);

#pragma endregion

#pragma region player

#define PLAYER_NAME_LEN			64

struct _Client;

typedef struct Player {

	const struct _Client *client;
	char name[PLAYER_NAME_LEN];

} Player;

extern void chat_player_create (const struct _Client *client);

#pragma endregion

#pragma region main

extern unsigned int chat_init (void);

extern void chat_end (void);

#pragma endregion

#pragma region handlers

extern void chat_on_name_received (
	struct _Client *client,
	const char *name, const size_t name_size
);

extern void chat_on_text_received (
	struct _Client *original_client,
	const char *text, const size_t text_size
);

#pragma endregion

#endif