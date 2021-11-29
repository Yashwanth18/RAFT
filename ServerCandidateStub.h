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


class ServerCandidateStub: public ServerSocket{

public:
    ServerCandidateStub() {};

    /* ---------------Candidate helper functions ----------*/
    void Stub_Handle_Poll_Candidate(std::vector<pollfd> *_pfds_server, ServerState * serverState,
                               std::map<int,int> *PeerIdIndexMap, bool *VoteRequest_Completed,
                               NodeInfo *nodeInfo);

    void Handle_AppendEntryRequest(NodeInfo *nodeInfo, ServerState *serverState, char *buf);
    void Handle_ResponseVote(NodeInfo *nodeInfo, ServerState *serverState, char *buf,
                             bool *VoteRequest_Completed, std::map<int,int> *PeerIdIndexMap);

    int Stub_SendVoteRequest(ServerState *serverState, NodeInfo *nodeInfo, int fd);
    void FillVoteRequest(ServerState * serverState, NodeInfo *nodeInfo, VoteRequest *VoteRequest);

};
