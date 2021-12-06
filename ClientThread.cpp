#include <iostream>
#include "ClientThread.h"

ClientThreadClass::ClientThreadClass() {}


void ClientThreadClass::
ThreadBody(std::vector<Peer_Info> *PeerServerInfo,  std::map<int,int> *PeerIdIndexMap,
           int _customerId, int _num_orders, int _requestType, std::mutex *print_lck) {

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

        for (int i = 0; i < num_orders; i++) {
            request.SetOrder(customer_id, i, request_type);

            timer.Start();
            write_success = stub.Order_WriteRequest(&request);
            timer.EndAndMerge();

            if (!write_success){
                std::cout << "write request failed: " << write_success << '\n';
            }
        }

    }
    else if (request_type == READ_REQUEST){
        socket_status = Connect_ServerRandomIndex(PeerServerInfo, &serverIndex);

        if (socket_status){
            request.SetOrder(customer_id, 1, request_type);
            stub.ReadRecord(&request, &record);

            print_lck -> lock();   // lock
            record.Print();
            print_lck -> unlock();   // lock
        }

        else{
            std::cout << "Server selected down"<< '\n';
        }
    }

    else if (request_type == LEADER_ID_REQUEST){     // ask who the leader is
        Connect_ServerRandomIndex(PeerServerInfo, &serverIndex);
        request.SetOrder(customer_id, num_orders, request_type);
        stub.Order_LeaderID(request, &leaderID);
        std::cout << "leaderID: " << leaderID << '\n';
    }

    else{
        std::cout << "Undefined requestType"<< '\n';
    }
}


bool::ClientThreadClass::
Connect_ServerRandomIndex(std::vector<Peer_Info> *PeerServerInfo,
                          int *serverIndex){

    int socket_status = 0;

    while(!socket_status){
        srand(time(0)); /* arbitrary */
        *serverIndex= rand() % (PeerServerInfo -> size());

        socket_status = Connect_ServerIndex(PeerServerInfo, *serverIndex);

//        std::cout << "Talking to server ID: " <<
//                    PeerServerInfo -> at(*serverIndex).unique_id<< '\n';
    }
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
            socket_status = Connect_ServerIndex(PeerServerInfo, serverIndex);
            // std::cout << "Connected to serverID: " << leaderID << '\n';
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
