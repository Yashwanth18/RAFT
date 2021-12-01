#include "ServerOutStub.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

/* return value: 0 on failure and 1 on success */
bool ServerOutStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}

int ServerOutStub::Read_MessageType() {
    int net_messageType;
    int messageType;
    char buf[sizeof (int)];

    if (!socket.Recv(buf, sizeof(int), 0)){
        perror("Read_MessageType");
        return 0;
    }

    memcpy(&net_messageType, buf, sizeof(net_messageType));
    messageType = ntohl(net_messageType);
    return messageType;
}

bool ServerOutStub::Send_MessageType(int messageType) {
    char buf[sizeof (int)];
    int send_status;
    int net_messageType = htonl(messageType);

    memcpy(buf, &net_messageType, sizeof(net_messageType));
    send_status = socket.Send(buf, sizeof (int), 0);
    return send_status;
}

/*--------------------------Candidate Helper Functions---------------------------- */

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
    int term = serverState -> currentTerm;
    int candidateId = nodeInfo -> node_id;
    int lastLogIndex = serverState -> smr_log.size() - 1;
    int lastLogTerm = serverState -> smr_log.back().logTerm;

    VoteRequest -> Set(term, candidateId, lastLogIndex, lastLogTerm);
}


/*--------------------------Leader Helper Functions---------------------------- */


int ServerOutStub::
SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo, int fd, int peer_index, int logRep_ID) {
    AppendEntryRequest appendEntryRequest;
    char buf[sizeof(AppendEntryRequest)];
    int send_status;

    FillAppendEntryRequest(serverState, nodeInfo, &appendEntryRequest, peer_index, logRep_ID);

    appendEntryRequest.Marshal(buf);
    send_status = socket.Send(buf, sizeof(AppendEntryRequest), 0);

    return send_status;
}

void ServerOutStub::FillAppendEntryRequest(ServerState * serverState, NodeInfo * nodeInfo,
                                              AppendEntryRequest *appendEntryRequest,  int peer_index, int logRep_ID) {

    int _sender_term = serverState -> currentTerm;
    int _leaderId = nodeInfo -> node_id;
    int _leaderCommit = serverState -> commitIndex;

    int _prevLogIndex = serverState -> nextIndex[peer_index] - 1;
    int _prevLogTerm = -1;
    LogEntry logEntry;

    int last_log_index = serverState -> smr_log.size() -1;
    int nextIndexPeer = serverState -> nextIndex[peer_index];

    if (nextIndexPeer > last_log_index){
        perror("nextIndexPeer out of range");
    }

    if (logRep_ID == -1){
        logEntry = LogEntry{-1, -1, -1, -1};
    }
    else{
        logEntry = serverState -> smr_log.at(nextIndexPeer);
    }

    if (_prevLogIndex >= 0){
        _prevLogTerm = serverState -> smr_log.at(_prevLogIndex).logTerm;
    }

    appendEntryRequest -> Set(_sender_term, _leaderId, _prevLogTerm, _prevLogIndex,
                              &logEntry, _leaderCommit, logRep_ID);
}










