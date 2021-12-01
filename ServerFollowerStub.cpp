#include "ServerFollowerStub.h"
#include <iostream>

ServerFollowerStub::ServerFollowerStub() {}

void ServerFollowerStub::Init(std::unique_ptr<ServerSocket> socket) {
	this->socket = std::move(socket);
}



int ServerFollowerStub::
Handle_VoteRequest(ServerState *serverState, NodeInfo *nodeInfo, char *buf) {

    VoteRequest voteRequest;
    ResponseVote ResponseVote;
    int success;
    int send_status;

    voteRequest.Unmarshal(buf);
    voteRequest.Print();

    success = Decide_Vote(serverState, &voteRequest);
    ResponseVote.Set(RESPONSE_VOTE, serverState -> currentTerm, success, nodeInfo -> node_id);
    send_status = SendResponseVote(&ResponseVote);

    return send_status;
}


int ServerFollowerStub::SendResponseVote(ResponseVote *ResponseVote) {
    int size = ResponseVote -> Size();
    char buf[size];
    int send_status;

    ResponseVote -> Marshal(buf);
    send_status = socket -> Send(buf, size, 0);

    return send_status;
}


bool ServerFollowerStub::
Decide_Vote(ServerState *serverState, VoteRequest *VoteRequest) {

    int result = false;
    int local_term = serverState -> currentTerm;
    int remote_term = VoteRequest -> Get_term();

    if (remote_term <= local_term){
        return false;
    }
    else {
        serverState -> votedFor = -1;
    }

    if ( Compare_Log(serverState, VoteRequest) && serverState -> votedFor == -1){
        result = (remote_term > local_term);
    }

    if (result){
        serverState -> votedFor = VoteRequest -> Get_candidateId();
        serverState -> currentTerm = VoteRequest -> Get_term();
    }

    return result;
}

/* Comparing the last_term and log length for the candidate node and the follower node */
bool ServerFollowerStub::Compare_Log(ServerState *serverState, VoteRequest * VoteRequest) {

    int candidate_last_log_term = VoteRequest -> Get_last_log_term();
    int candidate_last_log_index = VoteRequest -> Get_last_log_index();

    int local_last_log_term = serverState -> smr_log.back().logTerm;
    int local_last_log_index = serverState -> smr_log.size() -1;

    bool greater_last_log_term = (candidate_last_log_term > local_last_log_term);
    bool check_last_log_index = (candidate_last_log_index >= local_last_log_index);

    bool result = false;

    if (greater_last_log_term){
        result = true;
    }

    else if (candidate_last_log_term == local_last_log_term){
        return check_last_log_index;
    }
    return result;
}
