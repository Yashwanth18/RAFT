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
                         std::vector<std::thread> *thread_vector, ServerTimer *timer,
                         MapClientRecord *mapRecord);


	void IncomingThread(std::unique_ptr<ServerSocket> socket,
                        ServerState *serverState, ServerTimer *timer,
                        MapClientRecord *mapRecord);

    void CandidateThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                         NodeInfo *nodeInfo, ServerState *serverState);

    int Candidate_Quest(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                    NodeInfo *nodeInfo, ServerState *serverState);

    void LeaderThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                      NodeInfo *nodeInfo, ServerState *serverState,
                      MapClientRecord *mapRecord);

    void Apply_Committed_Op(ServerState *serverState,
                            MapClientRecord *mapRecord);

    bool Check_UpToDate(ServerState *serverState, int peer_index);


};
#endif // end of #ifndef __SERVERTHREAD_H__
