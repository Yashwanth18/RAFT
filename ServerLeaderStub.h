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


class ServerLeaderStub: public ServerSocket{

public:
    ServerLeaderStub() {};

    void Stub_Handle_Poll_Leader(std::vector<pollfd> *_pfds_server, NodeInfo *nodeInfo,
                                 ServerState *serverState, std::map<int,int> *PeerIdIndexMap, int * RequestID);

    int Stub_SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo,
                               int fd, int peer_index, int RequestID);

    void FillAppendEntryRequest(ServerState * serverState, NodeInfo * nodeInfo,
                                AppendEntryRequest *appendEntryRequest,  int peer_index,
                                int RequestID);

    void Handle_ResponseAppendEntry(char *buf, NodeInfo *nodeInfo,
                                    ServerState *serverState, std::map<int, int> *PeerIdIndexMap,
                                    int *RequestID, int socket_index);

    void Handle_AppendEntryRequest(ServerState *serverState, NodeInfo *nodeInfo, char *buf);

    void Handle_ResponseVote(ServerState *serverState, NodeInfo *nodeInfo, char *buf);

    void Handle_VoteRequest(ServerState *serverState, NodeInfo *nodeInfo, char *buf);

};
