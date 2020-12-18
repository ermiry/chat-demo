#ifndef _CHAT_PACKETS_H_
#define _CHAT_PACKETS_H_

#include <stddef.h>

#define PACKET_BUFFER_SIZE			2048

typedef enum PacketType {

	PACKET_TYPE_NONE			= 0,
	PACKET_TYPE_TEXT			= 1,
	PACKET_TYPE_NAME			= 2,
	PACKET_TYPE_CARDS			= 3,
	PACKET_TYPE_SELECTED_CARD	= 4,
	PACKET_TYPE_GAME_STARTED	= 5,
	PACKET_TYPE_GAME_ENDED		= 6,

} PacketType;

typedef struct PacketHeader {

	PacketType type;
	size_t packet_len;

} PacketHeader;

extern int packet_send_text (
	int sock_fd,
	const char *text, const size_t text_len
);

extern int packet_send_game_started (
	int sock_fd
);

extern int packet_send_game_ended (
	int sock_fd
);

extern int packet_send_initial_cards (
	int sock_fd,
	int card_1, int card_2, int card_3, int card_4, int card_5
);

#endif