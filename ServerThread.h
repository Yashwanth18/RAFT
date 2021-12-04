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

public:
    Raft(){}

    void ListeningThread(ServerSocket *serverSocket, ServerState *serverState,
                         std::vector<std::thread> *thread_vector,
                         std::mutex *lk_serverState, ServerTimer *timer);


	void IncomingThread(std::unique_ptr<ServerSocket> socket,
                        ServerState *serverState,
                        std::mutex *lk_serverState, ServerTimer *timer);

    void CandidateThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                         NodeInfo *nodeInfo, ServerState *serverState,
                         std::mutex *lk_serverStater);

    int Candidate_Quest(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                    NodeInfo *nodeInfo, ServerState *serverState,
                    std::mutex *lk_serverState);

    void LeaderThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                      NodeInfo *nodeInfo, ServerState *serverState,
                      std::mutex *lk_serverState);

    void Apply_Committed_Op(ServerState *serverState,  std::map<int, int> MapCustomerRecord,
                            std::mutex *lk_serverState, std::mutex *lk_Map);


};
#endif // end of #ifndef __SERVERTHREAD_H__
