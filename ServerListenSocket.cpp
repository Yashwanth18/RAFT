#include "ServerListenSocket.h"


int ServerListenSocket::Init(int port) {
	struct sockaddr_in addr;

	fd_ = socket(AF_INET, SOCK_STREAM, 0);


	if (fd_ < 0) {
		perror("ERROR: failed to create a socket");
		return false;
	}

	fcntl(fd_, F_SETFL, O_NONBLOCK);   //set socket to non-blocking

	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if ((bind(fd_, (struct sockaddr *) &addr, sizeof(addr))) < 0) {
		perror("ERROR: failed to bind");
		return false;
	}

	listen(fd_, 8);


	return fd_;
}

// std::unique_ptr<ServerListenSocket> ServerListenSocket::Accept() {
// 	int accepted_fd;
// 	struct sockaddr_in addr;
// 	unsigned int addr_size = sizeof(addr);
// 	accepted_fd = accept(fd_, (struct sockaddr *) &addr, &addr_size);
//
// 	if (accepted_fd < 0) {
// 		perror("ERROR: failed to accept connection");
// 		return nullptr;
// 	}
//
// 	return std::unique_ptr<ServerListenSocket>(new ServerListenSocket(accepted_fd, IsNagleOn()));
// }
