#include <string>
#include "ServerOutSocket.h"
#include "Messages.h"
#include <map>

class ServerOutStub {
private:
	ServerOutSocket socket;
public:
	ServerOutStub(){}

    bool Init(std::string ip, int port);

    bool Send_MessageType(int messageType);
    int Read_MessageType();     // return the messageType


    bool Handle_ResponseVote(NodeInfo *nodeInfo, ServerState *serverState);
    bool Send_RequestVote(ServerState *serverState, NodeInfo *nodeInfo);
    void FillVoteRequest(ServerState * serverState, NodeInfo * nodeInfo,
                         VoteRequest *VoteRequest);

    /* ------------------------Leader Helper Functions----------------------------*/
    bool SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo,
                               int peer_index, int heartbeat);

    void FillAppendEntryRequest(ServerState * serverState, NodeInfo * nodeInfo,
                                AppendEntryRequest *appendEntryRequest,  int peer_index,
                                int heartbeat);

    void Handle_ResponseAppendEntry(ServerState *serverState, int peer_index);
};
