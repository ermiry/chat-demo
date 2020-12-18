#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>

#include "client.h"
#include "network.h"
#include "packets.h"
#include "thread.h"

static Client *chat_client = NULL;

static void chat_client_receive_threads (void *client_ptr);

#pragma region main

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

int chat_client_init (
	const char *ip, int port
) {

	int retval = 1;

	chat_client = client_new ();
	if (chat_client) {
		chat_client->sock_fd = socket (AF_INET, SOCK_STREAM, 0);

		struct sockaddr_in *addr = (struct sockaddr_in *) &chat_client->address;
		addr->sin_family = AF_INET;
		addr->sin_addr.s_addr = inet_addr (ip);
		addr->sin_port = htons (port);

		retval = 0;
	}

	return retval;

}

static int connection_try (void) {

	int retval = 1;

	static const int max_sleep = 30;
	for (int numsec = 2; numsec <= max_sleep; numsec <<= 1) {
		if (!connect (chat_client->sock_fd,
			(const struct sockaddr *) &chat_client->address,
			sizeof (struct sockaddr))
		) {
			retval = 0;
			break;
		}

		if (numsec <= max_sleep / 2) sleep (numsec);
	}

	return retval;

}

int chat_client_connect_and_start (void) {

	int retval = 1;

	if (!connection_try ()) {
		chat_client->running = true;

		(void) thread_create_detachable (
			&chat_client->thread_id,
			(void *(*) (void *)) chat_client_receive_threads,
			chat_client
		);

		retval = 0;
	}

	return retval;

}

int chat_client_send_name (const char *name, const size_t name_len) {

	return packet_send_name (
		chat_client->sock_fd,
		name, name_len
	);

}

int chat_client_send_text (const char *text, const size_t text_len) {

	return packet_send_text (
		chat_client->sock_fd,
		text, text_len
	);

}

int chat_client_send_card (int card_value) {

	return packet_send_selected_card (chat_client->sock_fd, card_value);

}

void chat_client_end (void) {

	if (chat_client) {
		(void) close (chat_client->sock_fd);
		chat_client->sock_fd = -1;
		chat_client->running = false;

		client_delete (chat_client);
	}

}

#pragma endregion

#pragma region handler

static void chat_client_receive_handle_packet (
	Client *client,
	const char *buffer, const size_t buffer_size
) {

	char *end = (char *) buffer;

	PacketHeader *header = (PacketHeader *) end;

	end += sizeof (PacketHeader);

	switch (header->type) {
		case PACKET_TYPE_NONE: break;

		case PACKET_TYPE_TEXT: {
			// (void) printf ("[%d]: %.*s\n", client->sock_fd, (int) header->packet_len, end);
			(void) printf ("%.*s\n", (int) header->packet_len, end);
		} break;

		default: break;
	}

}

static inline int chat_client_receive_threads_actual (
	Client *client,
	char *buffer, const size_t buffer_size
) {

	int retval = 1;
	
	ssize_t rc = recv (client->sock_fd, buffer, buffer_size, 0);
	switch (rc) {
		case -1: {
			if (errno == EAGAIN) {
                // (void) printf (
                //     "chat_client_receive_threads () - sock fd %d timed out\n",
				// 	client->sock_fd
                // );

				retval = 0;
			}

			else {
				(void) printf (
                    "chat_client_receive_threads () - rc < 0 - sock fd %d error!\n",
					client->sock_fd
                );

				perror ("Error ");
			}
		} break;

		case 0: {
			(void) printf (
                "chat_client_receive_threads () - rc == 0 - sock fd %d error!\n",
                client->sock_fd
            );
		} break;

		default: {
			chat_client_receive_handle_packet (
				client,
				buffer, buffer_size
			);

			retval = 0;
		} break;
	}

	return retval;

}

static void chat_client_receive_threads (void *client_ptr) {

    Client *client = (Client *) client_ptr;

    (void) printf ("STARTED chat_client_receive_threads () - client %d\n", client->sock_fd);

    (void) sock_set_timeout (client->sock_fd, 5);

    size_t buffer_size = PACKET_BUFFER_SIZE;
	char *buffer = (char *) calloc (buffer_size, sizeof (char));
    if (buffer) {
        // TODO: handle failed
        while (
			chat_client->running
			&& !chat_client_receive_threads_actual (client, buffer, buffer_size)
		);

        free (buffer);
    }

    (void) printf ("\nENDED chat_client_receive_threads () - client %d\n", client->sock_fd);

}


#pragma endregion