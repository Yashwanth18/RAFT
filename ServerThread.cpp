#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerFollowerStub.h"
#include "ServerAdminStub.h"


void Election::
FollowerThread(std::unique_ptr<ServerSocket> socket, NodeInfo *nodeInfo, ServerState *serverState) {

    ServerFollowerStub serverFollowerStub;
    serverFollowerStub.Init(std::move(socket));

    int max_data_size = sizeof(AppendEntryRequest) + sizeof(ResponseAppendEntry) +
                        sizeof(VoteRequest) + sizeof(ResponseVote);
    char buf[max_data_size];
    int messageType;

    messageType = serverFollowerStub.Get_MessageType(buf, max_data_size);

    if (messageType == VOTE_REQUEST){ // main functionality
        serverFollowerStub.Handle_VoteRequest(serverState, nodeInfo, buf);
    }

}


void Election::
CandidateThread() {

}


void Election::
LeaderThread() {

}