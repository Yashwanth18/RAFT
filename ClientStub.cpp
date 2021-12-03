#include "ClientStub.h"

ClientStub::ClientStub() {}

int ClientStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}




int ClientStub::Order_LeaderID(CustomerRequest order, int *LeaderID) {
    char buf[sizeof(CustomerRequest)];
    int socket_status;
    int net_LeaderID;

    order.Marshal(buf);
    socket_status = socket.Send(buf, sizeof(CustomerRequest), 0);

    if (socket_status == 1) {
        socket_status = socket.Recv(buf, sizeof (int), 0);

        if (socket_status) {
            memcpy(&net_LeaderID, buf, sizeof(net_LeaderID));
            *LeaderID = ntohl(net_LeaderID);
        }
    }
    return socket_status;
}