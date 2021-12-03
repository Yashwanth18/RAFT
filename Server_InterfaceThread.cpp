#include <iostream>
#include <memory>

#include "Server_InterfaceThread.h"

void Interface::
Listening_Client(ServerSocket *clientSocket, ServerState *serverState,
                 std::mutex *lk_serverState, std::vector<std::thread> *thread_vector) {
    while (true) {
        std::unique_ptr<ServerSocket> new_socket;
        new_socket = clientSocket -> Accept();
        std::cout << "Accepted Connection from client" << '\n';

        std::thread newThread(&Interface::NewThread, this,
                              std::move(new_socket), serverState,
                              lk_serverState);

        thread_vector -> push_back(std::move(newThread));
    }
}

void Interface::
NewThread(std::unique_ptr<ServerSocket> socket, ServerState *serverState,
          std::mutex *lk_serverState){

    ServerIncomingStub inStub;
    inStub.Init(std::move(socket));
    CustomerRequest request;
    int requestType;
    int leaderID;

    while(true){
        request = inStub.ReceiveOrder();
        requestType = request.GetRequestType();
        std::cout << "requestType: " << requestType << '\n';

        if (!request.IsValid()) {
            break;
        }

        if (requestType == LEADER_ID_REQUEST){
            std::cout << "Received request_type: LEADER_ID_REQUEST" << '\n';

            lk_serverState -> lock();       // lock
            leaderID = serverState -> leader_id;
            lk_serverState -> unlock();     // unlock
            inStub.Send_LeaderID(leaderID);
        }

    }
}




