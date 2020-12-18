#include <stdlib.h>
#include <stdio.h>

#include <time.h>
#include <signal.h>

#include "chat.h"
#include "cerver.h"

static void quit (int dummy) {
	
	chat_cerver_end ();

	chat_end ();

	exit (0);

}

int main (int argc, const char **argv) {

	srand (time (NULL));

	signal (SIGINT, quit);
	signal (SIGTERM, quit);
	signal (SIGKILL, quit);

	signal (SIGPIPE, SIG_IGN);

	chat_version_print ();

	if (!chat_init ()) {
		if (!chat_cerver_init (
			7000,
			chat_player_create,
			chat_on_name_received,
			chat_on_text_received
		)) {
			if (chat_cerver_start ()) {
				(void) printf ("\nFailed to START cerver!\n\n");
			}
		}

		else {
			(void) printf ("\nFailed to INIT cerver!\n\n");
		}
	}

	else {
		(void) printf ("\nFailed to INIT chat!\n\n");
	}

	return 0;

}