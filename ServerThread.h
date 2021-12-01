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
#include "ServerTimer.h"

class Raft {

private:
    std::mutex lock_serverState;
    std::mutex lock_print;
public:
    Raft(){}
	void FollowerThread(std::unique_ptr<ServerSocket> socket,
                        ServerState *serverState, ServerTimer *timer);

    void CandidateThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                         NodeInfo *nodeInfo, ServerState *serverState);

    void LeaderThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                      NodeInfo *nodeInfo, ServerState *serverState);

    void Follower_ListeningThread(ServerSocket *serverSocket, ServerState *serverState,
                                  std::vector<std::thread> *thread_vector,
                                  ServerTimer *timer);

};
#endif // end of #ifndef __SERVERTHREAD_H__
