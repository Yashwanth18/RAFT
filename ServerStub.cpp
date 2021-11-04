#include "ServerStub.h"
#include <iostream>

ServerStub::ServerStub() {
	conn_descriptor = -1;
}

void ServerStub::Init(std::unique_ptr<ServerSocket> socket) {
	this->socket = std::move(socket);
}

int ServerStub::Get_Connection_Descriptor(){
	return conn_descriptor;
}

/* return value:
-1 on failure
0 if Recv return 0 (remote connection closed)
1 on success
*/

int ServerStub::Set_Connection_Descriptor(){
	char buffer[32];
	int net_conn_descriptor;
	int status;

	status = socket -> Recv(buffer, sizeof(net_conn_descriptor), 0);

	if(status){
		memcpy(&net_conn_descriptor, buffer, sizeof(net_conn_descriptor));
		conn_descriptor = ntohl(net_conn_descriptor);		//set conn_descriptor

		//send acknowledgement back to the remote connection
		int net_ack_status = htonl(1);
		memcpy(buffer, &net_ack_status, sizeof(net_ack_status));

		status = socket -> Send(buffer, sizeof(net_ack_status), 0);
	}

	return status;
}

/*-------------------------IFA Role---------------------------------*/


/* return value:
-1 on failure
0 if Recv return 0 (remote connection closed)
1 on success

Fill in the object rep_req
*/
int ServerStub::Receive_Replication_Request(ReplicationRequest * rep_req){
	char buffer[32];
	int status;

	status = socket -> Recv(buffer, rep_req -> Size(), 0);
	if (status) {
		rep_req -> Unmarshal(buffer);
	}
	return status;
}

/* return value:-1 on failure and 1 on success */
int ServerStub:: Ship_Success_Status(){
	char buffer [4];
	int status;
	int net_ack_status;

	net_ack_status = htonl(1);
	memcpy(buffer, &net_ack_status, sizeof(net_ack_status));

	status = socket -> Send(buffer, sizeof(net_ack_status), 0);

	return status;
}

/*-------------------------Engineer Role---------------------------------*/

/*NEED TO DO FAILURE HANDLING FOR RECV*/
CustomerRequest ServerStub::ReceiveOrder() {
	char buffer[32];
	CustomerRequest order;
	if (socket->Recv(buffer, order.Size(), 0)) {
		order.Unmarshal(buffer);
	}
	return order;
}

int ServerStub::ReturnRecord(CustomerRecord record){
	char buffer[32];
	record.Marshal(buffer);
	return socket->Send(buffer, record.Size(), 0);
}

/* return value:
-1 on failure, 1 on success
*/
int ServerStub::SendLaptop(LaptopInfo info) {
	int status;
	char buffer[32];

	info.Marshal(buffer);
	status = socket->Send(buffer, info.Size(), 0);
	return status;
}
