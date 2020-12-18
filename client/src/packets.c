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

int packet_send_name (
	int sock_fd,
	const char *name, const size_t name_len
) {

	int retval = 1;

	if (name) {
		char buffer[PACKET_BUFFER_SIZE] = { 0 };

		char *end = buffer;

		PacketHeader *header = (PacketHeader *) end;
		header->type = PACKET_TYPE_NAME;
		header->packet_len = name_len;

		end += sizeof (PacketHeader);

		(void) strncpy (end, name, PACKET_BUFFER_SIZE - sizeof (PacketHeader));

		ssize_t sent = send (sock_fd, buffer, PACKET_BUFFER_SIZE, 0);
		if (sent > 0) {
			retval = 0;
		}
	}

	return retval;

}

int packet_send_selected_card (
	int sock_fd, int card_value
) {

	int retval = 1;

	char buffer[PACKET_BUFFER_SIZE] = { 0 };

	char *end = buffer;

	PacketHeader *header = (PacketHeader *) end;
	header->type = PACKET_TYPE_SELECTED_CARD;
	header->packet_len = sizeof (int);

	end += sizeof (PacketHeader);

	*end = card_value;

	ssize_t sent = send (sock_fd, buffer, PACKET_BUFFER_SIZE, 0);
	if (sent > 0) {
		retval = 0;
	}

	return retval;

}