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
    std::mutex lock_state;
public:
	Election(){}
	void FollowerThread(std::unique_ptr<ServerSocket> socket, NodeInfo *nodeInfo,
                        ServerState *serverState);

    void CandidateThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                         NodeInfo *nodeInfo, ServerState *serverState, bool *sent);

    void LeaderThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                      NodeInfo *nodeInfo, ServerState *serverState, bool *sent);

};
#endif // end of #ifndef __SERVERTHREAD_H__
