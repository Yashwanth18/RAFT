#include "ClientStub.h"

ClientStub::ClientStub() {}

int ClientStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}

/* return value:
1 on success, and 0 if failure or remote connection closed from Recv
*/

int ClientStub:: Send_Connection_Descriptor(){
	char buffer[32];
	int net_connection_descriptor;
	int status;

	net_connection_descriptor = htonl(CUST_CONNECTED);
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
int ClientStub::OrderLaptop(CustomerRequest order, LaptopInfo * info) {
	char buffer[32];
	int size;
	int status;

	order.Marshal(buffer);
	size = order.Size();
	status = socket.Send(buffer, size, 0);
	if (status == 1) {
		size = info -> Size();
		status = socket.Recv(buffer, size, 0);

		if (status == 1) {
			info -> Unmarshal(buffer);
		}
	}
	return status;
}

/* return value:
1 on success, and 0 if failure or remote connection closed from Recv
*/

int ClientStub::
ReadRecord (CustomerRequest order, CustomerRecord * record){
	char buffer[32];
	int size;
	int status;

	order.Marshal(buffer);
	size = order.Size();
	status = socket.Send(buffer, size, 0);
	if (status == 1) {
		size = record -> Size();
		status = socket.Recv(buffer, size, 0);
		if (status == 1) {
			record -> Unmarshal(buffer);
		}
	}
	return status;
}
