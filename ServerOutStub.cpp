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
SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo, int peer_index, int logRepID) {
    AppendEntryRequest appendEntryRequest;
    char buf[sizeof(AppendEntryRequest)];
    int send_status;

    FillAppendEntryRequest(serverState, nodeInfo, &appendEntryRequest, peer_index, logRepID);

    appendEntryRequest.Marshal(buf);
    send_status = socket.Send(buf, sizeof(AppendEntryRequest), 0);

    return send_status;
}

void ServerOutStub::FillAppendEntryRequest(ServerState * serverState, NodeInfo * nodeInfo,
                                              AppendEntryRequest *appendEntryRequest,  int peer_index, int logRepID) {

    int sender_term = serverState -> currentTerm;
    int leaderId = nodeInfo -> node_id;
    int leaderCommit = serverState -> commitIndex;

    int prevLogIndex = serverState -> nextIndex[peer_index] - 1;
    int prevLogTerm = -1;
    LogEntry logEntry;

    int last_log_index = serverState -> smr_log.size() -1;
    int nextIndexPeer = serverState -> nextIndex[peer_index];

    /* to-do: break this into smaller functions: Set_LogEntry() or something*/
    if (logRepID == -1){           // heartbeat
        logEntry = LogEntry{-1, -1, -1, -1};
    }
    else{

        if (nextIndexPeer > last_log_index){
            perror("nextIndexPeer out of range");
        }
        logEntry = serverState -> smr_log.at(nextIndexPeer);
    }

    if (prevLogIndex >= 0){
        prevLogTerm = serverState -> smr_log.at(prevLogIndex).logTerm;
    }

    appendEntryRequest -> Set(sender_term, leaderId, prevLogTerm, prevLogIndex,
                              &logEntry, leaderCommit, logRepID);
}

void ServerOutStub::Handle_ResponseAppendEntry(ServerState *serverState, int peer_index) {
    char buf[sizeof (ResponseAppendEntry)];
    ResponseAppendEntry responseAppendEntry;
    int remote_term;
    int local_term;

    if (!socket.Recv(buf, sizeof(ResponseAppendEntry), 0)){
        perror("Handle_ResponseAppendEntry: Recv"); // to-do: handle error gracefully here
    }

    responseAppendEntry.Unmarshal(buf);
    remote_term = responseAppendEntry.Get_term();
    local_term = serverState -> currentTerm;

    if (remote_term > local_term){   // check if we are stale
        std::cout << "remote_term: " << remote_term << '\n';
        std::cout << "local_term: " << local_term << '\n';
        std::cout << "Handle_ResponseEntry: Leader Resigning to be a follower "<< '\n';

        serverState -> role = FOLLOWER;
        serverState -> currentTerm = responseAppendEntry.Get_term();
    }

    if (responseAppendEntry.Get_ResponseID() == 0){ // HEARTBEAT defined as 0
        std::cout << "Leader received heartbeat ack "<< '\n';
        responseAppendEntry.Print();
    }

    else{   // if response from proper log replication request
        std::cout << "Leader received ResponseAppendEntry" << '\n';
        responseAppendEntry.Print();

        if (responseAppendEntry.Get_success()) {
            serverState -> nextIndex[peer_index] ++;
        }
        else {  /* rejected: the follower node lags behind */
            serverState -> nextIndex[peer_index] --;
        }
    }

}








