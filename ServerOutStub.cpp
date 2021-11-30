#include "ServerOutStub.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

ServerOutStub::ServerOutStub() {}

/* return value: 0 on failure and 1 on success */
bool ServerOutStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}

/* */
int ServerOutStub::Send_RequestVote(ServerState *serverState, NodeInfo *nodeInfo) {
    VoteRequest VoteRequest;
    int send_status;
    int size = VoteRequest.Size();
    char buf[size];

    FillVoteRequest(serverState, nodeInfo, &VoteRequest);

    VoteRequest.Marshal(buf);
    send_status = socket.Send(buf, size, 0);

    return send_status;
}

void ServerOutStub::FillVoteRequest(ServerState * serverState, NodeInfo * nodeInfo,
                                      VoteRequest *VoteRequest) {
    int messageType = VOTE_REQUEST;
    int term = serverState -> currentTerm;
    int candidateId = nodeInfo -> node_id;
    int lastLogIndex = serverState -> smr_log.size() - 1;
    int lastLogTerm = serverState -> smr_log.back().logTerm;

    VoteRequest -> Set(messageType, term, candidateId, lastLogIndex, lastLogTerm);
}