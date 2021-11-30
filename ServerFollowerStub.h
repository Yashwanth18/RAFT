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

    int Get_MessageType(char *buf, int max_data_size);

    /* Election */
    int Handle_VoteRequest(ServerState *serverState, NodeInfo *nodeInfo, char *buf);
    int SendResponseVote(ResponseVote *ResponseVote);
    bool Decide_Vote(ServerState *serverState, VoteRequest *VoteRequest);
    bool Compare_Log(ServerState *serverState, VoteRequest * VoteRequest);

};

#endif // end of #ifndef __SERVER_STUB_H__
