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
                          MapClientRecord *mapRecord);

    void InterfaceThread(std::unique_ptr<ServerSocket> socket, ServerState *serverState,
                         MapClientRecord *mapRecord);

    void Fill_Customer_Record(CustomerRequest * request, CustomerRecord * record,
                              MapClientRecord *mapRecord);


};
#endif // end of #ifndef __SERVERCLIENTTHREAD_H__
