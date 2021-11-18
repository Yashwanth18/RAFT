#include "Socket.h"

#include <iostream>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/types.h>

Socket::Socket() : is_initialized_(false), nagle_(NAGLE_ON) {}

Socket::~Socket() {
	if (is_initialized_) {
		Close();
	}
}
int Socket::GetFd_() {
    return fd_;
}
/*
return 1 if success and 0 if failure
*/
int Socket::Send(char *buffer, int size, int flags) {
	int bytes_written = 0;
	int offset = 0;

	while (size > 0) {
		try{
			bytes_written = send(fd_, buffer + offset, size, flags);

			if (bytes_written < 0) {
				throw bytes_written;
			}
		}
		catch (int stat){
			Close();
			return 0;
		}

		size -= bytes_written;
		offset += bytes_written;
		assert(size >= 0);
	}

	return 1;
}

/*
return 1 if success and 0 if failure or remote connection lost
*/

int Socket::Recv(char *buffer, int size, int flags) {
	int bytes_read = 0;
	int offset = 0;
	while (size > 0) {
		try{
			bytes_read = recv(fd_, buffer + offset, size, flags);
			if (bytes_read <= 0) {
				throw bytes_read;
			}
		}
		catch (int stat){
			Close();
			return 0;
		}

		assert(bytes_read != 0);

		size -= bytes_read;
		offset += bytes_read;
		assert(size >= 0);
	}

	return 1;
}


void Socket::Close() {
	close(fd_);
	is_initialized_ = false;
}

int Socket::NagleOn(bool on_off) {
	nagle_ = (on_off ? NAGLE_ON : NAGLE_OFF);
	int result = setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY,
				(void *) &nagle_, sizeof(int));
				
	if (result < 0) {
		perror("ERROR: setsockopt failed");
		return 0;
	}
	return 1;
}

bool Socket::IsNagleOn() {
	return (nagle_ == NAGLE_ON) ? true : false;
}
