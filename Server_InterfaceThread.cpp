#include <iostream>
#include <memory>

#include "Server_InterfaceThread.h"

void Interface::
Listening_Client(ServerSocket *clientSocket, ServerState *serverState,
                 std::mutex *lk_serverState, std::vector<std::thread> *thread_vector,
                 std::map<int, int> *MapCustomerRecord, std::mutex *lk_MapRecord) {
    while (true) {
        std::unique_ptr<ServerSocket> new_socket;
        new_socket = clientSocket -> Accept();
        std::cout << "Accepted Connection from client" << '\n';

        std::thread interfaceThread(&Interface::InterfaceThread, this, std::move(new_socket),
                              serverState, lk_serverState, MapCustomerRecord,
                              lk_serverState);

        thread_vector -> push_back(std::move(interfaceThread));
    }
}

void Interface::
InterfaceThread(std::unique_ptr<ServerSocket> socket, ServerState *serverState,
                std::mutex *lk_serverState, std::map<int, int> *MapCustomerRecord,
                std::mutex *lk_MapRecord){

    ServerIncomingStub inStub;
    inStub.Init(std::move(socket));
    CustomerRequest request;
    CustomerRecord record;
    int requestType;
    int leaderID;

    while(true){
        request = inStub.ReceiveOrder();
        requestType = request.GetRequestType();

        if (!request.IsValid()) {
            break;
        }

        std::cout << "requestType: " << requestType << '\n';
        request.Print();

        switch (requestType) {
            case LEADER_ID_REQUEST:     /* MACRO defined to be number 4 */

                lk_serverState -> lock();       // lock
                leaderID = serverState -> leader_id;
                lk_serverState -> unlock();     // unlock
                inStub.Send_LeaderID(leaderID);
                break;

            case WRITE_REQUEST:     /* MACRO defined to be number 1 */

                // check if we are the leader here
                // notify the leaderThread

                break;

            case READ_REQUEST:      /* MACRO defined to be number 2 */
                Fill_Customer_Record(&request, &record, MapCustomerRecord, lk_MapRecord);
                inStub.ReturnRecord(record);
                break;

            default:
                std::cout << "Undefined request_type: " << requestType << '\n';
        }
    }
}


void Interface::
Fill_Customer_Record(CustomerRequest * request, CustomerRecord * record,
                     std::map<int, int> *MapCustomerRecord, std::mutex *lk_MapRecord){

    std::map<int,int>::iterator iter;

    lk_MapRecord -> lock();		//lock Map
    iter = MapCustomerRecord -> find(request -> GetCustomerId());

    if (iter == (*MapCustomerRecord).end()){				//record does not exist in map
        record -> SetCustomerId(-1);
        record -> SetLastOrder(-1);
    }
    else{
        record -> SetCustomerId(request -> GetCustomerId());
        record -> SetLastOrder( (*MapCustomerRecord)[ request -> GetCustomerId() ]);
    }

    lk_MapRecord -> unlock();	//unlock Map
}



