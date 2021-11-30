#include <string>
#include "ServerAdminSocket.h"
#include "Messages.h"

class ServerAdminStub {
private:
	ServerAdminSocket socket;
public:
	ServerAdminStub();

	bool Init(std::string ip, int port);

    int Send_RequestVote(ServerState *serverState, NodeInfo *nodeInfo);
    void FillVoteRequest(ServerState * serverState, NodeInfo * nodeInfo, VoteRequest *VoteRequest);
};
