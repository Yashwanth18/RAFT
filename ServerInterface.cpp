#include <iostream>
#include <memory>

#include "ServerInterface.h"

void Interface::
Listening_Client(ServerSocket *clientSocket, ServerState *serverState,
                 std::vector<std::thread> *thread_vector,
                 Map_Customer_Record *mapCustomerRecord) {

    while (true) {
        std::unique_ptr<ServerSocket> new_socket;
        new_socket = clientSocket -> Accept();
        std::cout << "Accepted Connection from client" << '\n';

        std::thread interfaceThread(&Interface::InterfaceThread, this, std::move(new_socket),
                              serverState, mapCustomerRecord);

        thread_vector -> push_back(std::move(interfaceThread));
    }
}

void Interface::
InterfaceThread(std::unique_ptr<ServerSocket> socket, ServerState *serverState,
                Map_Customer_Record *mapCustomerRecord){

    ServerInStub inStub;
    CustomerRequest request;
    CustomerRecord record;

    int requestType;
    int leaderID;
    int role_;
    int rep_success;

    inStub.Init(std::move(socket));

    while(true){
        request = inStub.ReceiveOrder();
        requestType = request.GetRequestType();

        if (!request.IsValid()) {
            break;
        }

        request.Print();

        switch (requestType) {

            case WRITE_REQUEST: {    /* MACRO defined to be number 1 */
                serverState->lck.lock();       // lock
                role_ = serverState->role;
                serverState->lck.unlock();     // unlock
                if (role_ != LEADER) { break; }



                rep_success = 1; // to-do
                inStub.Send_Ack(rep_success);
                break;
            }
            case READ_REQUEST: {      /* MACRO defined to be number 2 */
                Fill_Customer_Record(&request, &record, mapCustomerRecord);
                inStub.ReturnRecord(record);
                break;
            }
            case LEADER_ID_REQUEST: {     /* MACRO defined to be number 4 */
                serverState->lck.lock();       // lock
                leaderID = serverState->leader_id;
                serverState->lck.unlock();     // unlock

                inStub.Send_LeaderID(leaderID);
                std::cout << "leaderID: " << leaderID << '\n';
                break;
            }
            default:
                std::cout << "Undefined request_type: " << requestType << '\n';
        }
    }
}


void Interface::
Fill_Customer_Record(CustomerRequest * request, CustomerRecord * record,
                     Map_Customer_Record *mapCustomerRecord){

    std::map<int,int>::iterator iter;
    std::map<int, int> CustomerRecord_dict = mapCustomerRecord -> CustomerRecord_dict;

    mapCustomerRecord -> lck.lock();		// lock
    iter = CustomerRecord_dict.find(request -> GetCustomerId());

    if (iter == CustomerRecord_dict.end()){				// record does not exist in map
        record -> SetCustomerId(-1);
        record -> SetLastOrder(-1);
    }
    else{
        record -> SetCustomerId(request -> GetCustomerId());
        record -> SetLastOrder( CustomerRecord_dict[ request -> GetCustomerId() ]);
    }
    mapCustomerRecord -> lck.unlock();	// Unlock
}





