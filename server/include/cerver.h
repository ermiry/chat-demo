#ifndef _CHAT_CERVER_H_
#define _CHAT_CERVER_H_

#include <stdbool.h>

#include <time.h>

#include <pthread.h>

#include <netinet/in.h>

#pragma region client

struct _Client {

	time_t connected_timestamp;

	int sock_fd;
	struct sockaddr_storage address;

	pthread_t thread_id;

};

typedef struct _Client Client;

#pragma endregion

#pragma region cerver

#define CERVER_MAX_CLIENTS				16

typedef struct Cerver {

	int sock;
	struct sockaddr_storage address;
	int port;

	bool running;

	int n_clients;
	Client *clients[CERVER_MAX_CLIENTS];

	// events
	void (*on_client_connected)(const Client *);
	void (*on_name_received)(
		Client *client,
		const char *name, const size_t name_size
	);
	void (*on_text_received)(
		Client *client,
		const char *text, const size_t text_size
	);

} Cerver;

// inits cerver internal values
// returns 0 on success, 1 on error
extern int chat_cerver_init (
    int port,
    void (*on_client_connected)(const Client *),
	void (*on_name_received)(
		Client *client,
		const char *name, const size_t name_size
	),
	void (*on_text_received)(
		Client *client,
		const char *text, const size_t text_size
	)
);

// enables connections to chat cerver
// returns 0 on success, 1 on error
extern int chat_cerver_start (void);

extern void chat_cerver_broadcast_text_to_clients (
	const char *text, const size_t text_size
);

extern void chat_cerver_broadcast_text_to_remaining_clients (
	Client *original_client,
	const char *text, const size_t text_size
);

extern void chat_cerver_broadcast_game_started (void);

extern void chat_cerver_broadcast_game_ended (void);

// closes cerver connections & deletes internal values
extern void chat_cerver_end (void);

#pragma endregion

#endif