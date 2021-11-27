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
    void Add_Socket_To_Poll(int new_fd);

    /* ---------------Candidate helper functions ----------*/
    void Handle_Poll_Candidate(ServerState * serverState, std::map<int,int> *PeerIdIndexMap,
                               bool *request_completed, NodeInfo *nodeInfo);
    int SendRequestVote(ServerState *serverState, NodeInfo *nodeInfo, int fd);


    /* ----------------------------------Leader helper function ------------------------------------*/
    void Handle_Poll_Leader(ServerState *serverState, std::map<int,int> *PeerIdIndexMap, int * RequestID);
    int SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo,
                               int fd, int peer_index, int RequestID);


    /* -------------------------------------Follower helper function ------------------------------------*/
    void Handle_Poll_Follower(ServerState *serverState, NodeInfo *nodeInfo);
};
