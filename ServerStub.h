#include <poll.h>
#include <vector>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <map>

#include "ServerListenSocket.h"
#include "ServerTimer.h"
#include "Messages.h"
#include "ServerSocket.h"
#include "ServerCandidateStub.h"
#include "ServerFollowerStub.h"
#include "ServerLeaderStub.h"

class ServerStub: public ServerSocket{
private:
    ServerListenSocket ListenSocket;
    std::vector<pollfd> pfds_server;    /* for checking sockets connected to peer server. */

    ServerFollowerStub serverFollowerStub;
    ServerCandidateStub serverCandidateStub;
    ServerLeaderStub serverLeaderStub;
public:

    ServerStub() {};
    void Init(NodeInfo * nodeInfo);
    int Poll(int poll_timeout);          /* Poll_timeout is in millisecond */
    void Set_Pfd(int new_fd, int peer_index);


    /* ---------------Candidate helper functions ----------*/
    void Handle_Poll_Candidate(ServerState * serverState, std::map<int,int> *PeerIdIndexMap,
                               bool *VoteRequest_Completed, NodeInfo *nodeInfo,
                               int *Socket, bool *Is_Init, bool *Socket_Status);

    int SendVoteRequest(ServerState *serverState, NodeInfo *nodeInfo, int fd);


    /* ----------------------------------Leader helper function ------------------------------------*/
    void Handle_Poll_Leader(ServerState *serverState, NodeInfo *nodeInfo , std::map<int,int> *PeerIdIndexMap,
                            int * LogRep_RequestID, int *Socket, bool *Is_Init, bool *Socket_Status);

    int SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo,
                               int fd, int peer_index, int LogRep_RequestID);


    /* -------------------------------------Follower helper function ------------------------------------*/
    void Handle_Poll_Follower(ServerTimer *Timer, ServerState *serverState, NodeInfo *nodeInfo,
                              int *Socket, bool *Is_Init, bool *Socket_Status);
};
