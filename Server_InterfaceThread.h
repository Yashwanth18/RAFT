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
                          std::mutex *lk_serverState, std::vector<std::thread> *thread_vector);

    void NewThread(std::unique_ptr<ServerSocket> socket, ServerState *serverState,
                   std::mutex *lk_serverState);
};
#endif // end of #ifndef __SERVERCLIENTTHREAD_H__
