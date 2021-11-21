#include "ClientListenSocket.h"


int ClientListenSocket::Init(int port) {
	struct sockaddr_in addr;
//    int reuse_socket;

	fd_ = socket(AF_INET, SOCK_STREAM, 0);


	if (fd_ < 0) {
		perror("ERROR: failed to create a socket");
		return false;
	}

	fcntl(fd_, F_SETFL, O_NONBLOCK);   // set socket to non-blocking

	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if ((bind(fd_, (struct sockaddr *) &addr, sizeof(addr))) < 0) {
		perror("ERROR: failed to bind");
		return false;
	}

//    if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuse_socket, sizeof(reuse_socket)) == -1){
//        perror("setsockopt");
//        exit(1);
//    }

	listen(fd_, 8);


	return fd_;
}
