#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <netinet/in.h>

#include <sys/socket.h>

#include "cerver.h"
#include "network.h"
#include "packets.h"
#include "thread.h"

static void chat_cerver_receive_threads (void *client_ptr);

#pragma region client

static Client *client_new (void) {

	Client *client = (Client *) malloc (sizeof (Client));
	if (client) {
		(void) memset (client, 0, sizeof (Client));
	}

	return client;

}

static void client_delete (void *client_ptr) {

	if (client_ptr) {
		free (client_ptr);
	}

}

#pragma endregion

#pragma region cerver

static Cerver *chat_cerver = NULL;

static Cerver *cerver_new (void) {

	Cerver *cerver = (Cerver *) malloc (sizeof (Cerver));
	if (cerver) {
		(void) memset (cerver, 0, sizeof (Cerver));

        for (int i = 0; i < CERVER_MAX_CLIENTS; i++) {
            cerver->clients[i] = NULL;
        }

        cerver->on_client_connected = NULL;
		cerver->on_name_received = NULL;
		cerver->on_text_received = NULL;
	}

	return cerver;

}

static void cerver_delete (void *cerver_ptr) {

	if (cerver_ptr) {
        Cerver *cerver = (Cerver *) cerver_ptr;

        for (int i = 0; i < CERVER_MAX_CLIENTS; i++) {
			if (cerver->clients[i]) client_delete (cerver->clients[i]);
            cerver->clients[i] = NULL;
        }

        cerver->on_client_connected = NULL;
		cerver->on_name_received = NULL;
		cerver->on_text_received = NULL;

		free (cerver_ptr);
	}

}

// inits cerver internal values
// returns 0 on success, 1 on error
int chat_cerver_init (
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
) {

	int retval = 1;

	chat_cerver = cerver_new ();
	if (chat_cerver) {
		chat_cerver->port = port;

        chat_cerver->on_client_connected =  on_client_connected;
		chat_cerver->on_name_received = on_name_received;
		chat_cerver->on_text_received = on_text_received;

		retval = 0;
	}

	return retval;

}

static int chat_cerver_start_internal (void) {

	int retval = 1;

	chat_cerver->sock = socket (AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in *addr = (struct sockaddr_in *) &chat_cerver->address;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_port = htons (chat_cerver->port);

	if (!bind (chat_cerver->sock, (const struct sockaddr *) &chat_cerver->address, sizeof (struct sockaddr_storage))) {
		retval = 0;       // success!!
	}

	else {
		(void) printf ("Failed to bind pocket cerver socket!");

		(void) close (chat_cerver->sock);
	}

	return retval;

}

static void chat_cerver_register_client (Client *client) {

    for (int i = 0; i < CERVER_MAX_CLIENTS; i++) {
        if (chat_cerver->clients[i] == NULL) {
            chat_cerver->clients[i] = client;
			break;
        }
    }

	chat_cerver->n_clients += 1;

}

static void chat_cerver_accept (void) {

    static struct sockaddr_storage client_address = { 0 };
    static socklen_t socklen = sizeof (struct sockaddr_storage);

    (void) memset (&client_address, 0, sizeof (struct sockaddr_storage));

    // accept the new connection
    int new_fd = accept (chat_cerver->sock, (struct sockaddr *) &client_address, &socklen);
    if (new_fd > 0) {
        (void) printf ("Accepted fd: %d\n", new_fd);
        
        // create a new client
        Client *client = client_new ();
        if (client) {
            client->connected_timestamp = time (NULL);
            client->sock_fd = new_fd;
            (void) memcpy (&client->address, &client_address, sizeof (struct sockaddr_storage));

            // register client to cerver
            chat_cerver_register_client (client);

            // event handler
            chat_cerver->on_client_connected (client);

            // handle client values
            (void) thread_create_detachable (
                &client->thread_id,
                (void *(*) (void *)) chat_cerver_receive_threads,
                client
            );
        }
    }

    else {
        (void) printf ("Accept failed!\n");
        perror ("Error");
        (void) printf ("\n");
    }

}

// enables connections to chat cerver
// returns 0 on success, 1 on error
int chat_cerver_start (void) {

	int retval = 1;

	if (chat_cerver) {
		if (!chat_cerver_start_internal ()) {
			if (!listen (chat_cerver->sock, 8)) {
				chat_cerver->running = true;

				(void) printf ("\nChat cerver is ready in port %d!\n", chat_cerver->port);
				(void) printf ("Waiting for connections...\n");
                
				while (chat_cerver->running) {
                    // accept the connections
                    chat_cerver_accept ();
				}
			}
		}
	}

	return retval;

}

void chat_cerver_broadcast_text_to_clients (
	const char *text, const size_t text_size
) {

	for (int i = 0; i < CERVER_MAX_CLIENTS; i++) {
		if (chat_cerver->clients[i]) {
			packet_send_text (
				chat_cerver->clients[i]->sock_fd,
				text, text_size
			);
		}
	}

}

void chat_cerver_broadcast_text_to_remaining_clients (
	Client *original_client,
	const char *text, const size_t text_size
) {

	for (int i = 0; i < CERVER_MAX_CLIENTS; i++) {
		if (chat_cerver->clients[i]) {
			if (chat_cerver->clients[i]->sock_fd != original_client->sock_fd) {
				packet_send_text (
					chat_cerver->clients[i]->sock_fd,
					text, text_size
				);
			}
		}
	}

}

void chat_cerver_broadcast_game_started (void) {

	for (int i = 0; i < CERVER_MAX_CLIENTS; i++) {
		if (chat_cerver->clients[i]) {
			packet_send_game_started (chat_cerver->clients[i]->sock_fd);
		}
	}

}

void chat_cerver_broadcast_game_ended (void) {

	for (int i = 0; i < CERVER_MAX_CLIENTS; i++) {
		if (chat_cerver->clients[i]) {
			packet_send_game_ended (chat_cerver->clients[i]->sock_fd);
		}
	}

}

// closes cerver connections & deletes internal values
void chat_cerver_end (void) {

	if (chat_cerver) {
		cerver_delete (chat_cerver);
	}

}

#pragma endregion

#pragma region handler

static void chat_cerver_receive_handle_packet (
	Client *client,
	const char *buffer, const size_t buffer_size
) {

	char *end = (char *) buffer;

	PacketHeader *header = (PacketHeader *) end;

	end += sizeof (PacketHeader);

	switch (header->type) {
		case PACKET_TYPE_NONE: break;

		case PACKET_TYPE_NAME: {
			chat_cerver->on_name_received (
				client,
				end, header->packet_len
			);
		} break;

		case PACKET_TYPE_TEXT: {
			chat_cerver->on_text_received (
				client,
				end, header->packet_len
			);
		} break;

		default: break;
	}

}

static inline int chat_cerver_receive_threads_actual (
	Client *client,
	char *buffer, const size_t buffer_size
) {

	int retval = 1;
	
	ssize_t rc = recv (client->sock_fd, buffer, buffer_size, 0);
	switch (rc) {
		case -1: {
			if (errno == EAGAIN) {
                // (void) printf (
                //     "chat_cerver_receive_threads () - sock fd %d timed out\n",
				// 	client->sock_fd
                // );

				retval = 0;
			}

			else {
				(void) printf (
                    "chat_cerver_receive_threads () - rc < 0 - sock fd %d error!\n",
					client->sock_fd
                );

				perror ("Error ");
			}
		} break;

		case 0: {
			(void) printf (
                "chat_cerver_receive_threads () - rc == 0 - sock fd %d error!\n",
                client->sock_fd
            );
		} break;

		default: {
			chat_cerver_receive_handle_packet (
				client,
				buffer, buffer_size
			);

			retval = 0;
		} break;
	}

	return retval;

}

static void chat_cerver_receive_threads (void *client_ptr) {

    Client *client = (Client *) client_ptr;

    (void) printf ("STARTED chat_cerver_receive_threads () - client %d\n", client->sock_fd);

    (void) sock_set_timeout (client->sock_fd, 5);

    size_t buffer_size = PACKET_BUFFER_SIZE;
	char *buffer = (char *) calloc (buffer_size, sizeof (char));
    if (buffer) {
        // TODO: handle failed
        while (
			chat_cerver->running
			&& !chat_cerver_receive_threads_actual (client, buffer, buffer_size)
		);

        free (buffer);
    }

    (void) printf ("\nENDED chat_cerver_receive_threads () - client %d\n", client->sock_fd);

}

#pragma endregion