#include <stdio.h>

#include <pthread.h>

int thread_create_detachable (pthread_t *thread, void *(*work) (void *), void *args) {

	int retval = 1;

	if (thread) {
		pthread_attr_t attr = { 0 };
		if (!pthread_attr_init (&attr)) {
			if (!pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED)) {
				if (!pthread_create (thread, &attr, work, args) != 0) {
					retval = 0;     // success
				}

				else {
					(void) printf ("Failed to create detachable thread!\n");
					perror ("Error");
				}
			}
		}
	}

	return retval;

}