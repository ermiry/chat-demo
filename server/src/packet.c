#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "packets.h"

int packet_send_text (
	int sock_fd,
	const char *text, const size_t text_len
) {

	int retval = 1;

	if (text) {
		char buffer[PACKET_BUFFER_SIZE] = { 0 };

		char *end = buffer;

		PacketHeader *header = (PacketHeader *) end;
		header->type = PACKET_TYPE_TEXT;
		header->packet_len = text_len;

		end += sizeof (PacketHeader);

		(void) strncpy (end, text, PACKET_BUFFER_SIZE - sizeof (PacketHeader));

		ssize_t sent = send (sock_fd, buffer, PACKET_BUFFER_SIZE, 0);
		if (sent > 0) {
			retval = 0;
		}
	}

	return retval;

}

int packet_send_game_started (
	int sock_fd
) {

	int retval = 1;

	char buffer[PACKET_BUFFER_SIZE] = { 0 };

	char *end = buffer;

	PacketHeader *header = (PacketHeader *) end;
	header->type = PACKET_TYPE_GAME_STARTED;

	ssize_t sent = send (sock_fd, buffer, PACKET_BUFFER_SIZE, 0);
	if (sent > 0) {
		retval = 0;
	}

	return retval;

}

int packet_send_game_ended (
	int sock_fd
) {

	int retval = 1;

	char buffer[PACKET_BUFFER_SIZE] = { 0 };

	char *end = buffer;

	PacketHeader *header = (PacketHeader *) end;
	header->type = PACKET_TYPE_GAME_ENDED;

	ssize_t sent = send (sock_fd, buffer, PACKET_BUFFER_SIZE, 0);
	if (sent > 0) {
		retval = 0;
	}

	return retval;

}

int packet_send_initial_cards (
	int sock_fd,
	int card_1, int card_2, int card_3, int card_4, int card_5
) {

	int retval = 1;

	char buffer[PACKET_BUFFER_SIZE] = { 0 };

	char *end = buffer;

	PacketHeader *header = (PacketHeader *) end;
	header->type = PACKET_TYPE_CARDS;
	header->packet_len = sizeof (int) * 5;

	end += sizeof (PacketHeader);

	int *card_ptr_1 = (int *) end;
	*card_ptr_1 = card_1;
	end += sizeof (int);

	int *card_ptr_2 = (int *) end;
	*card_ptr_2 = card_2;
	end += sizeof (int);

	int *card_ptr_3 = (int *) end;
	*card_ptr_3 = card_3;
	end += sizeof (int);

	int *card_ptr_4 = (int *) end;
	*card_ptr_4 = card_4;
	end += sizeof (int);

	int *card_ptr_5 = (int *) end;
	*card_ptr_5 = card_5;

	ssize_t sent = send (sock_fd, buffer, PACKET_BUFFER_SIZE, 0);
	if (sent > 0) {
		retval = 0;
	}

	return retval;

}