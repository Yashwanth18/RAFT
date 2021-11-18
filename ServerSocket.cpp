#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <iostream>
#include "ServerSocket.h"

/* return
-1 if failure,
0 if already initialized,
1 if success
*/
socketInfo* ServerSocket::Init(std::string ip, int port) {
	int socket_status;
    socketInfo *socket_info = malloc(sizeof (socketInfo));
	if (is_initialized_) {
        socketInfo->socketStatus = 0;
		return socketInfo;
	}

	struct sockaddr_in addr;
	socketInfo->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketInfo->fd < 0) {
		perror("ERROR: failed to create a socket");
        socketInfo->socketStatus = -1;
		return socketInfo;
	}

	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	addr.sin_port = htons(port);

	/*BE CAREFUL: connect return 0 on success*/
	try{
		socketInfo->socket_status = connect(fd_, (struct sockaddr *) &addr, sizeof(addr));
		if (socketInfo->socket_status < 0){
			throw socketInfo->socket_status;
		}
	}
	catch(int stat){
        socketInfo->socketStatus = -1;
		return socketInfo;
	}

	is_initialized_ = true;
    socketInfo->socketStatus = 1;
    fcntl(socketInfo->fd, F_SETFL, O_NONBLOCK);
    return socketInfo;

}
