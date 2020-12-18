#ifndef _CHAT_CLIENT_H_
#define _CHAT_CLIENT_H_

#include <stdbool.h>

#include <time.h>
#include <pthread.h>

#include <netinet/in.h>

struct _Client {

	int sock_fd;
	struct sockaddr_storage address;

	bool running;

	pthread_t thread_id;

};

typedef struct _Client Client;

extern int chat_client_init (
	const char *ip, int port
);

extern int chat_client_connect_and_start (void);

extern int chat_client_send_name (const char *name, const size_t name_len);

extern int chat_client_send_text (const char *text, const size_t text_len);

extern void chat_client_end (void);

#endif