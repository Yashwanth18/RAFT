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

    /* Responding to Candidate */
    int Handle_VoteRequest(ServerState *serverState);
    int SendResponseVote(ResponseVote *ResponseVote);
    bool Decide_Vote(ServerState *serverState, VoteRequest *VoteRequest);
    bool Compare_Log(ServerState *serverState, VoteRequest * VoteRequest);

    /* Responding to Leader */
    int Handle_AppendEntryRequest(ServerState *serverState, NodeInfo *nodeInfo);
    int Send_ResponseAppendEntry(ResponseAppendEntry *ResponseAppendEntry);
    void Set_Leader(AppendEntryRequest *appendEntryRequest, ServerState *serverState, NodeInfo *nodeInfo);
    void Set_CommitIndex(AppendEntryRequest *appendEntryRequest, ServerState * serverState);
    bool Set_Result(ServerState *serverState, AppendEntryRequest *appendEntryRequest);

    void Print_Log(ServerState *serverState);
};

#endif // end of #ifndef __SERVER_STUB_H__
