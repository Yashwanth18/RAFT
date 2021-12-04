#include <iostream>
#include "ClientThread.h"

ClientThreadClass::ClientThreadClass() {}


void ClientThreadClass::
ThreadBody(std::vector<Peer_Info> *PeerServerInfo,  std::map<int,int> *PeerIdIndexMap,
           int _customerId, int _num_orders, int _requestType) {

    customer_id = _customerId;
    num_orders = _num_orders;
    request_type = _requestType;

    int socket_status;
    int leaderID;
    int serverIndex;
    int write_success;

    CustomerRequest request;
    CustomerRecord record;


    if (request_type == WRITE_REQUEST){
        Connect_Leader(PeerServerInfo, PeerIdIndexMap);
        request.SetOrder(customer_id, 1, request_type);

        write_success = stub.Order_WriteRequest(&request);
        std::cout << "write_success: " << write_success << '\n';
    }
    else if (request_type == READ_REQUEST){
        socket_status = Connect_ServerRandomIndex(PeerServerInfo, &serverIndex);

        if (socket_status){
            request.SetOrder(customer_id, 1, request_type);
            socket_status = stub.ReadRecord(&request, &record);
            record.Print();
        }

        else{
            std::cout << "Server selected down"<< '\n';
        }
    }

    else if (request_type == LEADER_ID_REQUEST){     // ask who the leader is
        socket_status = Connect_ServerRandomIndex(PeerServerInfo, &serverIndex);
        // to-do: make this robust. Keep connecting

        request.SetOrder(customer_id, num_orders, request_type);
        socket_status = stub.Order_LeaderID(request, &leaderID);
        std::cout << "leaderID: " << leaderID << '\n';
    }

    else{
        std::cout << "Undefined requestType"<< '\n';
    }
}


bool::ClientThreadClass::
Connect_ServerRandomIndex(std::vector<Peer_Info> *PeerServerInfo,
                          int *serverIndex){
    int socket_status;

    srand(time(0)); /* arbitrary */
    *serverIndex= rand() % (PeerServerInfo -> size());

    std::cout << "connecting to serverID: " <<
                    PeerServerInfo ->at(*serverIndex).unique_id<< '\n';

    socket_status = Connect_ServerIndex(PeerServerInfo, *serverIndex);

    return socket_status;
}

bool ClientThreadClass::Connect_ServerIndex(std::vector<Peer_Info> *PeerServerInfo,
                                            int index) {

    std::string ip;
    int port;

    ip = PeerServerInfo -> at(index).IP;
    port = PeerServerInfo -> at(index).port;
    return stub.Init(ip, port);
}

bool ClientThreadClass::
Connect_Leader(std::vector<Peer_Info> *PeerServerInfo,
               std::map<int, int> *PeerIdIndexMap) {

    int socket_status;
    int leaderID = -1;
    int serverIndex;
    CustomerRequest customerRequest;

    socket_status = Connect_ServerRandomIndex(PeerServerInfo, &serverIndex);

    if (socket_status){
        customerRequest.SetOrder(customer_id, -1, LEADER_ID_REQUEST);
        socket_status = stub.Order_LeaderID(customerRequest, &leaderID);
    }

    while (leaderID != (*PeerServerInfo)[serverIndex].unique_id){
        stub.Close_Socket();

        if(socket_status){
            serverIndex = (*PeerIdIndexMap)[leaderID];
            std::cout << "Connecting to serverID: " << leaderID << '\n';
            socket_status = Connect_ServerIndex(PeerServerInfo, serverIndex);
        }

        if (socket_status){
            socket_status = stub.Order_LeaderID(customerRequest, &leaderID);
        }

        /* if the server is down */
        while (!socket_status){  // keep trying until finding a live server
            stub.Close_Socket();
            socket_status = Connect_ServerRandomIndex(PeerServerInfo,
                                                      &serverIndex);

            if (socket_status){
                socket_status = stub.Order_LeaderID(customerRequest,
                                                    &leaderID);
            }
        } // End: while

    }

    return socket_status;

}






ClientTimer ClientThreadClass::GetTimer() {
    return timer;
}
