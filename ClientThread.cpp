#include <iostream>
#include "ClientThread.h"

ClientThreadClass::ClientThreadClass() {}


void ClientThreadClass::
ThreadBody(std::string ip, int port, int id, int orders, int type) {
    customer_id = id;
    num_orders = orders;
    request_type = type;
    int socket_status;
    int leaderID;

    CustomerRequest customerRequest;

    socket_status = stub.Init(ip, port);
    std::cout << "socket_status: " << socket_status << '\n';

    if (request_type == LEADER_ID_REQUEST){     // ask who the leader is

        customerRequest.SetOrder(customer_id, num_orders, request_type);

        socket_status = stub.Order_LeaderID(customerRequest, &leaderID);
        std::cout << "leaderID: " << leaderID << '\n';

        // to-do: implement dictionary serverID -> ServerIndex

    }
    else if (request_type == 2){
        CustomerRequest request;
        CustomerRecord record;
        request.SetOrder(customer_id, -1, request_type);

        socket_status = stub.ReadRecord(request, &record);
        record.Print();
    }

    else if (request_type == 3){

    } // End requestType 3
}




ClientTimer ClientThreadClass::GetTimer() {
    return timer;
}
