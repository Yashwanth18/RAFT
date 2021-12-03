#include <string>
#include "OutSocket.h"
#include "Messages.h"
#include <map>

class ServerOutStub {
private:
	OutSocket socket;
public:
	ServerOutStub(){}

    bool Init(std::string ip, int port);

    bool Send_MessageType(int messageType);
    int Read_MessageType();     // return the messageType


    bool Handle_ResponseVote(ServerState *serverState, std::mutex *lk_serverState);
    bool Send_RequestVote(ServerState *serverState, NodeInfo *nodeInfo);
    void FillVoteRequest(ServerState * serverState, NodeInfo * nodeInfo,
                         VoteRequest *VoteRequest);

    /* ------------------------Leader Helper Functions----------------------------*/
    bool SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo,
                               int peer_index, int heartbeat, std::mutex *lk_serverState);

    void FillAppendEntryRequest(ServerState * serverState, NodeInfo * nodeInfo,
                                AppendEntryRequest *appendEntryRequest,  int peer_index,
                                int heartbeat, std::mutex *lk_serverState);

    bool Handle_ResponseAppendEntry(ServerState *serverState, int peer_index,
                                    NodeInfo *nodeInfo, std::mutex *lk_serverState);

    void Update_CommitIndex(ServerState *serverState, NodeInfo *nodeInfo);
};
