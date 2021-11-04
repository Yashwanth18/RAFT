#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <iostream>
#include "ServerAdminSocket.h"

/* return
-1 if failure,
0 if already initialized,
1 if success
*/
int ServerAdminSocket::Init(std::string ip, int port) {
	int status;

	if (is_initialized_) {
		return 0;
	}

	struct sockaddr_in addr;
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_ < 0) {
		perror("ERROR: failed to create a socket");
		return -1;
	}

	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	addr.sin_port = htons(port);

	/*BE CAREFUL: connect return 0 on success*/
	try{
		status = connect(fd_, (struct sockaddr *) &addr, sizeof(addr));
		if (status < 0){
			throw status;
		}
	}
	catch(int stat){
		return -1;
	}

	is_initialized_ = true;
	return 1;

}
