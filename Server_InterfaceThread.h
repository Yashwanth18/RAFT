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
#include "ServerIncomingStub.h"

class Interface {
private:

public:
    Interface(){}

    void Listening_Client(ServerSocket *clientSocket,ServerState *serverState,
                          std::mutex *lk_serverState, std::vector<std::thread> *thread_vector,
                          std::map<int, int> *MapCustomerRecord, std::mutex *lk_MapRecord);

    void InterfaceThread(std::unique_ptr<ServerSocket> socket, ServerState *serverState,
                         std::mutex *lk_serverState, std::map<int, int> *MapCustomerRecord,
                         std::mutex *lk_MapRecord);

    void Fill_Customer_Record(CustomerRequest * request, CustomerRecord * record,
                              std::map<int, int> *MapCustomerRecord, std::mutex *lk_MapRecord);

};
#endif // end of #ifndef __SERVERCLIENTTHREAD_H__
