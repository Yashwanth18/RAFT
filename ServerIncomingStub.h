#ifndef __SERVER_STUB_H__
#define __SERVER_STUB_H__

#include <memory>
#include <cstring>
#include <arpa/inet.h>
#include "ServerSocket.h"
#include "Messages.h"


class ServerIncomingStub {
private:
	std::unique_ptr<ServerSocket> socket;
public:
	ServerIncomingStub();
	void Init(std::unique_ptr<ServerSocket> socket);

    int Read_MessageType();
    bool Send_MessageType(int messageType);

    /* ---------------------Responding to Candidate----------------------------------*/
    bool Handle_VoteRequest(ServerState *serverState, std::mutex *lk_serverState);
    bool SendResponseVote(ResponseVote *ResponseVote);
    bool Decide_Vote(ServerState *serverState, VoteRequest *VoteRequest);
    bool Compare_Log(ServerState *serverState, VoteRequest * VoteRequest);

    /* ----------------------Responding to Leader -----------------------------------------*/
    bool Handle_AppendEntryRequest(ServerState *serverState, std::mutex *lk_serverState);
    bool Send_ResponseAppendEntry(ResponseAppendEntry *ResponseAppendEntry);
    void Set_Leader(AppendEntryRequest *appendEntryRequest, ServerState *serverState);
    void Set_CommitIndex(AppendEntryRequest *appendEntryRequest, ServerState * serverState);
    bool Set_Result(ServerState *serverState, AppendEntryRequest *appendEntryRequest);
    bool Check_ConflictingLog(ServerState *serverState, AppendEntryRequest *appendEntryRequest);

    void Print_Log(ServerState *serverState);


    /*----Client Interface---------*/
    CustomerRequest ReceiveOrder();
    bool Send_LeaderID(int leaderID); // return socket_status;
};

#endif // end of #ifndef __SERVER_STUB_H__
