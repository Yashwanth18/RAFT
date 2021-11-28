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


class ServerFollowerStub: public ServerSocket{

public:
    ServerFollowerStub() {};

    /* ---------------Follower helper function ----------*/
    void Stub_Handle_Poll_Follower(std::vector<pollfd> *_pfds_server, ServerState *serverState, NodeInfo *nodeInfo);

    /* For Log Replication */
    int Send_ResponseAppendEntry(ResponseAppendEntry *ResponseAppendEntry, int fd);
    bool Set_Result(ServerState *serverState, AppendEntryRequest *appendEntryRequest);
    void Set_CommitIndex(AppendEntryRequest *appendEntryRequest, ServerState * serverState);
    void Print_Log(ServerState *serverState);

    /* For Log Replication */
    int SendVoteResponse(VoteResponse *voteResponse, int fd);
    int Decide_Vote(ServerState *serverState, NodeInfo *nodeInfo, RequestVote *requestVote);
    int Compare_Log(ServerState *serverState, NodeInfo * nodeInfo,RequestVote * requestVote);

};