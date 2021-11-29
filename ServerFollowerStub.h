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
    void Stub_Handle_Poll_Follower(ServerTimer *Timer, std::vector<pollfd> *_pfds_server, ServerState *serverState,
                                   NodeInfo *nodeInfo);

    void Handle_AppendEntryRequest(ServerState *serverState, NodeInfo *nodeInfo, char *buf, int fd);
    void Set_Leader(AppendEntryRequest *appendEntryRequest, ServerState *serverState, NodeInfo *nodeInfo);

    void Handle_VoteRequest(ServerState *serverState, NodeInfo *nodeInfo, char *buf, int fd);


    /* For Log Replication Module */
    int Send_ResponseAppendEntry(ResponseAppendEntry *ResponseAppendEntry, int fd);
    bool Set_Result(ServerState *serverState, AppendEntryRequest *appendEntryRequest);
    void Set_CommitIndex(AppendEntryRequest *appendEntryRequest, ServerState * serverState);
    void Print_Log(ServerState *serverState);

    /* For Election Module */
    int SendResponseVote(ResponseVote *ResponseVote, int fd);
    int Decide_Vote(ServerState *serverState, NodeInfo *nodeInfo, VoteRequest *VoteRequest);
    bool Compare_Log(ServerState *serverState, NodeInfo * nodeInfo,VoteRequest * VoteRequest);

};
