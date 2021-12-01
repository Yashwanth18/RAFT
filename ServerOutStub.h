#include <string>
#include "ServerOutSocket.h"
#include "Messages.h"

class ServerOutStub {
private:
	ServerOutSocket socket;
public:
	ServerOutStub(){}

    bool Init(std::string ip, int port);

    bool Send_MessageType(int messageType);
    int Read_MessageType();     // return the messageType


    int Send_RequestVote(ServerState *serverState, NodeInfo *nodeInfo);
    void FillVoteRequest(ServerState * serverState, NodeInfo * nodeInfo, VoteRequest *VoteRequest);

    /* Leader */
    int SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo,
                                    int fd, int peer_index, int logRep_ID);

    void FillAppendEntryRequest(ServerState * serverState, NodeInfo * nodeInfo,
                                AppendEntryRequest *appendEntryRequest,  int peer_index,
                                int logRep_ID);

};
