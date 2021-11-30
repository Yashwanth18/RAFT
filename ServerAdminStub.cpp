#include "ServerAdminStub.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

ServerAdminStub::ServerAdminStub() {}

/*return value:
-1 on failure, 0 if already initialized, and 1 on success
*/
int ServerAdminStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}


