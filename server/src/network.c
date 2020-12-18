#include <fcntl.h>

#include <sys/time.h>
#include <sys/socket.h>

int sock_set_timeout (int sock_fd, time_t timeout) {

	struct timeval tv = { 0 };
	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	return setsockopt (
		sock_fd, 
		SOL_SOCKET, SO_RCVTIMEO, 
		(const char *) &tv, sizeof (struct timeval)
	);

}