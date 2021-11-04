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


/* return value:
1 on success, and 0 if failure or remote connection closed from Recv
*/
int ServerAdminStub:: Send_Connection_Description(){
	char buffer[32];
	int net_connection_descriptor;
	int status;

	net_connection_descriptor = htonl(PFA_CONNECTED);
	memcpy(buffer, &net_connection_descriptor, sizeof(net_connection_descriptor));

	status = socket.Send(buffer, sizeof(net_connection_descriptor), 0);

	if (status){
		status = socket.Recv(buffer, sizeof(int), 0);
	}
	return status;
}


/* return value:
1 on success, and 0 if failure or remote connection closed from Recv
*/
int ServerAdminStub::Order_Replication_Request(ReplicationRequest * rep_req){
  char buffer[32];
  int size;
	int status = 1;

  rep_req -> Marshal(buffer);
  size = rep_req -> Size();

	status = socket.Send(buffer, size, 0);

	if (status){
		status = socket.Recv(buffer, sizeof(int), 0);
	}

	return status;
}
