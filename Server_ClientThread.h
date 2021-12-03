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

class Interface {
private:

public:
    Interface(){}

    void Listening_Client(ServerSocket *clientSocket);
};
#endif // end of #ifndef __SERVERCLIENTTHREAD_H__
