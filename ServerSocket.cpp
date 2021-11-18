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
SocketInfo* ServerSocket::Init(std::string ip, int port) {
	int socket_status;
    SocketInfo *socket_info ;
    //= malloc(sizeof (socketInfo));
	if (is_initialized_) {
        socket_info->socket_status = 0;
		return socket_info;
	}

	struct sockaddr_in addr;
    socket_info->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_info->fd < 0) {
		perror("ERROR: failed to create a socket");
        socket_info->socket_status = -1;
		return socket_info;
	}

	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	addr.sin_port = htons(port);

	/*BE CAREFUL: connect return 0 on success*/
	try{
        socket_info->socket_status = connect(fd_, (struct sockaddr *) &addr, sizeof(addr));
		if (socket_info->socket_status < 0){
			throw socket_info->socket_status;
		}
	}
	catch(int stat){
        socket_info->socket_status = -1;
		return socket_info;
	}

	is_initialized_ = true;
    socket_info->socket_status = 1;
    fcntl(socket_info->fd, F_SETFL, O_NONBLOCK);
    return socket_info;

}
