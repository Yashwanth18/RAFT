#include <iostream>
#include <memory>

#include "ServerInterface.h"

void Interface::
Listening_Client(ServerSocket *clientSocket, ServerState *serverState,
                 std::vector<std::thread> *thread_vector,
                 MapClientRecord *MapRecord) {

    while (true) {
        std::unique_ptr<ServerSocket> new_socket;
        new_socket = clientSocket -> Accept();
        // std::cout << "Accepted Connection from client" << '\n';

        std::thread interfaceThread(&Interface::InterfaceThread, this, std::move(new_socket),
                              serverState, MapRecord);

        thread_vector -> push_back(std::move(interfaceThread));
    }
}

void Interface::
InterfaceThread(std::unique_ptr<ServerSocket> socket, ServerState *serverState,
                MapClientRecord *MapRecord){

    ServerInStub inStub;
    CustomerRequest request;
    CustomerRecord record;

    int requestType;
    int leaderID;
    int rep_success;
    int opcode = 1;
    int arg1;
    int arg2;
    int nodeTerm;
    int request_index;

    inStub.Init(std::move(socket));

    while(true){
        request = inStub.ReceiveOrder();
        requestType = request.GetRequestType();
        arg1 = request.GetCustomerId();
        arg2 = request.GetOrderNumber();

        if (!request.IsValid()) {
            break;
        }
        // request.Print();

        switch (requestType) {

            case WRITE_REQUEST: {    /* MACRO defined to be number 1 */
                rep_success = 0;
                if (serverState -> GetRole() != LEADER) { break; }

                /* Notify the leaderThread */
                serverState->lck.lock();       // lock

                nodeTerm = serverState -> currentTerm;
                LogEntry logEntry{nodeTerm, opcode, arg1, arg2};
                serverState -> smr_log.push_back(logEntry);
                request_index = serverState -> smr_log.size() - 1;
                std::cout << "waiting for req: " << request_index << '\n';

                serverState->lck.unlock();     // unlock


                /* Wait until the request is considered Committed (not necessarily applied) */
                while (!rep_success){
                    serverState->lck.lock();       // lock
                    rep_success = (serverState -> commitIndex >= request_index);
                    serverState->lck.unlock();     // unlock
                }

                std::cout << "done request" << '\n';

                /* Send Acknowledgement back to client */
                inStub.Send_Ack(rep_success);
                break;
            }
            case READ_REQUEST: {      /* MACRO defined to be number 2 */
                Fill_Customer_Record(&request, &record, MapRecord);
                inStub.ReturnRecord(record);
                break;
            }
            case LEADER_ID_REQUEST: {     /* MACRO defined to be number 4 */
                leaderID = serverState->Get_leaderID();
                inStub.Send_LeaderID(leaderID);
                break;
            }
            default:
                std::cout << "Undefined request_type: " << requestType << '\n';
        }
    }
}


void Interface::
Fill_Customer_Record(CustomerRequest * request, CustomerRecord * record,
                     MapClientRecord *MapRecord){

    std::map<int,int>::iterator iter;

    MapRecord -> lck.lock();		// lock
    iter = MapRecord -> Record_Dict.find(request -> GetCustomerId());

    /* if record does not exist in map */
    if (iter == MapRecord -> Record_Dict.end()){
        record -> SetCustomerId(-1);
        record -> SetLastOrder(-1);
    }
    else{   // if record exists in map
        record -> SetCustomerId(request -> GetCustomerId());
        record -> SetLastOrder( MapRecord -> Record_Dict[request -> GetCustomerId()]);
    }
    MapRecord -> lck.unlock();	// Unlock
}





