#include "ClientStub.h"

ClientStub::ClientStub() {}

int ClientStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}

void ClientStub::Close_Socket() {
    socket.Close();
}


int ClientStub::Order_LeaderID(CustomerRequest order, int *LeaderID) {
    char buf[sizeof(CustomerRequest)];
    int socket_status;
    int net_LeaderID;

    order.Marshal(buf);
    socket_status = socket.Send(buf, sizeof(CustomerRequest), 0);

    if (socket_status) {
        socket_status = socket.Recv(buf, sizeof (int), 0);

        if (socket_status) {
            memcpy(&net_LeaderID, buf, sizeof(net_LeaderID));
            *LeaderID = ntohl(net_LeaderID);
        }
    }
    return socket_status;
}

int ClientStub::Order_WriteRequest(CustomerRequest *request) {
    char buf[sizeof(CustomerRequest)];
    int socket_status;
    int net_success;
    int write_success = 0;

    request -> Marshal(buf);
    socket_status = socket.Send(buf, sizeof(CustomerRequest), 0);

    if (socket_status) {
        socket_status = socket.Recv(buf, sizeof (int), 0);

        if (socket_status) {
            memcpy(&net_success, buf, sizeof(net_success));
            write_success = ntohl(net_success);
        }
        else{
            return 0;
        }
    }
    return write_success;
}


bool ClientStub::
ReadRecord (CustomerRequest * customerRequest, CustomerRecord * record){
    char buffer[32];
    bool socket_status;

    customerRequest -> Marshal(buffer);
    socket_status = socket.Send(buffer, sizeof(CustomerRequest), 0);

    if (socket_status) {
        socket_status = socket.Recv(buffer, sizeof(CustomerRecord), 0);

        if (socket_status) {
            record -> Unmarshal(buffer);
        }
    }
    return socket_status;
}
