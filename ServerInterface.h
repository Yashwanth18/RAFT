#ifndef __SERVERCLIENTTHREAD_H__
#define __SERVERCLIENTTHREAD_H__

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <map>

#include "Messages.h"
#include "ServerSocket.h"
#include "ServerTimer.h"
#include "ServerInStub.h"



class Interface {
private:

public:
    Interface(){}

    void Listening_Client(ServerSocket *clientSocket, ServerState *serverState,
                          std::vector<std::thread> *thread_vector,
                          Map_Customer_Record *mapCustomerRecord);

    void InterfaceThread(std::unique_ptr<ServerSocket> socket, ServerState *serverState,
                         Map_Customer_Record *mapCustomerRecord);

    void Fill_Customer_Record(CustomerRequest * request, CustomerRecord * record,
                              Map_Customer_Record *mapCustomerRecord);


};
#endif // end of #ifndef __SERVERCLIENTTHREAD_H__
