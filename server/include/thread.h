#ifndef _CHAT_THREADS_H_
#define _CHAT_THREADS_H_

#include <pthread.h>

extern int thread_create_detachable (
	pthread_t *thread,
	void *(*work) (void *), void *args
);

#endif