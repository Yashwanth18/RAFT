#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <iostream>
#include "ServerOutSocket.h"

/* Connect to the host with IP and port
 * return 0 if failure, and 1 if success */
bool ServerOutSocket::Init(std::string ip, int port) {
	int status;
    int fd;

	if (is_initialized_) {
		return true;
	}

	struct sockaddr_in addr;
    fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0) {
		perror("ERROR: ServerOutSocket failed to create a socket");
		return false;
	}


	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	addr.sin_port = htons(port);

	/*BE CAREFUL: connect return 0 on success*/
	try{
		status = connect(fd, (struct sockaddr *) &addr, sizeof(addr));
		if (status < 0){
			throw status;
		}
	}
	catch(int stat){
        close(fd);
		return false;
	}

	is_initialized_ = true;
    fd_ = fd;
	return true;

}
