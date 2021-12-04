#include <iostream>
#include <memory>

#include "ServerInterface.h"

void Interface::
Listening_Client(ServerSocket *clientSocket, ServerState *serverState,
                 std::vector<std::thread> *thread_vector,
                 Map_Customer_Record *mapCustomerRecord, Bridge *bridge) {

    while (true) {
        std::unique_ptr<ServerSocket> new_socket;
        new_socket = clientSocket -> Accept();
        std::cout << "Accepted Connection from client" << '\n';

        std::thread interfaceThread(&Interface::InterfaceThread, this, std::move(new_socket),
                              serverState, mapCustomerRecord, bridge);

        thread_vector -> push_back(std::move(interfaceThread));
    }
}

void Interface::
InterfaceThread(std::unique_ptr<ServerSocket> socket, ServerState *serverState,
                Map_Customer_Record *mapCustomerRecord, Bridge *bridge){

    ServerInStub inStub;
    CustomerRequest request;
    CustomerRecord record;
    std::promise<CustomerRequest> prom;
    std::future<CustomerRequest> fut = prom.get_future();
    std::unique_ptr<WriteRequest> req = std::unique_ptr<WriteRequest>(new WriteRequest);

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

                req->request = request;
                req->prom = std::move(prom);

                /* to-do: push to the queue and notify all leaderThreads */
                bridge->lck.lock();       // lock queue
                bridge->queue.push(std::move(req));
                bridge->cv.notify_all();
                bridge->lck.unlock();      // unlock queue

                // request = fut.get();     // wait for leaderThread to replicate and commit the write request

                std::cout << "queue size: " << bridge->queue.size() << '\n';
                /* Send Acknowledgement back to client */
                rep_success = 1;
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





