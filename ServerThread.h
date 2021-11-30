 #ifndef __SERVERTHREAD_H__
#define __SERVERTHREAD_H__

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <map>

#include "Messages.h"
#include "ServerSocket.h"

class Election {

private:

public:
	Election(){}
	void FollowerThread(std::unique_ptr<ServerSocket> socket);
    void CandidateThread();
    void LeaderThread();

};
#endif // end of #ifndef __SERVERTHREAD_H__
