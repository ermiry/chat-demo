#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <time.h>
#include <signal.h>

#include "client.h"
#include "input.h"
#include "chat.h"

static void quit (int dummy) {

	chat_client_end ();

	exit (0);

}

int main (int argc, const char **argv) {

	srand (time (NULL));

	(void) signal (SIGINT, quit);
	(void) signal (SIGTERM, quit);
	(void) signal (SIGKILL, quit);

	(void) signal (SIGPIPE, SIG_IGN);

	chat_version_print ();

	char *name_buffer = NULL;
	input_clean_stdin ();
	(void) printf ("Enter name: ");
	name_buffer = input_get_line ();

	fprintf (stdout, "\nConnecting to cerver...\n");

	if (!chat_client_init (
			"127.0.0.1", 7000
		)) {
		if (!chat_client_connect_and_start ()) {
			printf ("Connected to server!\n\n");

			chat_player_create (name_buffer);

			chat_client_send_name (name_buffer, strlen (name_buffer));

			// handle inputs
			char *input_buffer = NULL;
			size_t input_buffer_len = 0;
			while (1) {
				input_clean_stdin ();

				(void) printf ("Enter text: ");
				input_buffer = input_get_line ();

				if (input_buffer) {
					input_buffer_len = strlen (input_buffer);
					if (input_buffer_len > 0) {
						// send to cerver
						chat_client_send_text (
							input_buffer, input_buffer_len
						);
					}

					free (input_buffer);
				}
			}
		}

		else {
			(void) printf ("\nFailed to connect chat client!\n\n");
		}
	}

	else {
		(void) printf ("\nFailed to init chat client!\n\n");
	}

	return 0;

}