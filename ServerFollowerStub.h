#ifndef __SERVER_STUB_H__
#define __SERVER_STUB_H__

#include <memory>
#include <cstring>
#include <arpa/inet.h>
#include "ServerSocket.h"
#include "Messages.h"


class ServerFollowerStub {
private:
	std::unique_ptr<ServerSocket> socket;
public:
	ServerFollowerStub();
	void Init(std::unique_ptr<ServerSocket> socket);

    int Read_MessageType();
    bool Send_MessageType(int messageType);

    /* Election */
    int Handle_VoteRequest(ServerState *serverState, NodeInfo *nodeInfo);
    int SendResponseVote(ResponseVote *ResponseVote);
    bool Decide_Vote(ServerState *serverState, VoteRequest *VoteRequest);
    bool Compare_Log(ServerState *serverState, VoteRequest * VoteRequest);

};

#endif // end of #ifndef __SERVER_STUB_H__
