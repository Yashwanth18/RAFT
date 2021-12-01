#include <string>
#include "ServerOutSocket.h"
#include "Messages.h"

class ServerOutStub {
private:
	ServerOutSocket socket;
public:
	ServerOutStub();

	bool Init(std::string ip, int port);
int Read(char *buf, int size);
    int Send_RequestVote(ServerState *serverState, NodeInfo *nodeInfo);
    void FillVoteRequest(ServerState * serverState, NodeInfo * nodeInfo, VoteRequest *VoteRequest);
};
