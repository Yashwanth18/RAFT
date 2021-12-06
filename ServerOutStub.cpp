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
    int socket_status;

    socket_status = socket.Recv(buf, sizeof(int), 0);
    if (!socket_status){
        return 0;
    }

    memcpy(&net_messageType, buf, sizeof(net_messageType));
    messageType = ntohl(net_messageType);
    return messageType;
}

bool ServerOutStub::Send_MessageType(int messageType) {
    char buf[sizeof (int)];
    int socket_status;
    int net_messageType = htonl(messageType);

    memcpy(buf, &net_messageType, sizeof(net_messageType));
    socket_status = socket.Send(buf, sizeof (int), 0);
    return socket_status;
}

/*--------------------------Candidate Helper Functions---------------------------- */

bool ServerOutStub::Send_RequestVote(ServerState *serverState, NodeInfo *nodeInfo) {
    VoteRequest VoteRequest;
    bool socket_status;
    int size = VoteRequest.Size();
    char buf[size];

    FillVoteRequest(serverState, nodeInfo, &VoteRequest);

    VoteRequest.Marshal(buf);
    socket_status = socket.Send(buf, size, 0);

    return socket_status;
}

void ServerOutStub::FillVoteRequest(ServerState * serverState, NodeInfo * nodeInfo,
                                      VoteRequest *VoteRequest) {
    int term = serverState -> currentTerm;
    int candidateId = nodeInfo -> node_id;
    int lastLogIndex = serverState -> smr_log.size() - 1;
    int lastLogTerm = serverState -> smr_log.back().logTerm;

    VoteRequest -> Set(term, candidateId, lastLogIndex, lastLogTerm);
}

bool ServerOutStub::
Handle_ResponseVote(ServerState *serverState){

    ResponseVote responseVote;
    char buf[sizeof (ResponseVote)];
    bool socket_status;

    socket_status = socket.Recv(buf, sizeof(ResponseVote), 0);

    if (!socket_status){
        return false;
    }

    responseVote.Unmarshal(buf);
    responseVote.Print();

    if (responseVote.Get_voteGranted()) { /* vote granted */
        serverState -> Increment_numVote();
    }
    else {  /* vote got rejected */
        if (responseVote.Get_term() > serverState -> Get_nodeTerm()){ // we are stale
            serverState -> Set_nodeTerm(responseVote.Get_term());
        }
    }

    return socket_status;
}

/*--------------------------Leader Helper Functions---------------------------- */

bool ServerOutStub::
SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo, int peer_index,
                       int heartbeat) {

    AppendEntryRequest appendEntryRequest;
    char buf[sizeof(AppendEntryRequest)];
    bool socket_status;

    FillAppendEntryRequest(serverState, nodeInfo, &appendEntryRequest, peer_index,
                           heartbeat);

    appendEntryRequest.Marshal(buf);
    socket_status = socket.Send(buf, sizeof(AppendEntryRequest), 0);

    return socket_status;
}

void ServerOutStub::
FillAppendEntryRequest(ServerState * serverState, NodeInfo * nodeInfo,
                       AppendEntryRequest *appendEntryRequest,
                       int peer_index, int heartbeat) {

    int leaderId = nodeInfo -> node_id;
    int prevLogTerm = -1;

    serverState -> lck.lock();       // lock
    int sender_term = serverState -> currentTerm;
    int leaderCommit = serverState -> commitIndex;
    int prevLogIndex = serverState -> nextIndex[peer_index] - 1;

    int last_log_index = serverState -> smr_log.size() -1;
    int nextIndex = serverState -> nextIndex[peer_index];
    serverState -> lck.unlock();     // unlock

    LogEntry logEntry;
    if (heartbeat){
        logEntry = LogEntry{-1, -1, -1, -1};
    }

    else{   // if not heartbeat
        if (nextIndex > last_log_index){        // error checking
            perror("nextIndex out of range");
        }
        logEntry = serverState -> Get_LogEntry(nextIndex);
    }

    if (prevLogIndex >= 0){
        prevLogTerm = serverState -> Get_LogEntry(prevLogIndex).logTerm;
    }

    appendEntryRequest -> Set(sender_term, leaderId, prevLogTerm, prevLogIndex,
                              &logEntry, leaderCommit);
}

bool ServerOutStub::
Handle_ResponseAppendEntry(ServerState *serverState, int peer_index,
                           NodeInfo *nodeInfo) {

    char buf[sizeof (ResponseAppendEntry)];
    ResponseAppendEntry responseAppendEntry;
    int remote_term;
    int local_term;
    bool socket_status;

    socket_status = socket.Recv(buf, sizeof(ResponseAppendEntry), 0);

    if (!socket_status){
        return false;
    }

    responseAppendEntry.Unmarshal(buf);
    remote_term = responseAppendEntry.Get_term();
    local_term = serverState -> Get_nodeTerm();

    if (remote_term > local_term){   // check if we are stale
        std::cout << "\nStale Leader Resigning to be a follower "<< '\n';
        std::cout << "remote_term: " << remote_term << '\n';
        std::cout << "local_term: " << local_term << '\n' << '\n';

        serverState -> SetRole(FOLLOWER);
        serverState -> Set_nodeTerm(responseAppendEntry.Get_term());
    }


    if (responseAppendEntry.Get_success()) {    // success
        if (!responseAppendEntry.Get_Heartbeat()){
            serverState -> lck.lock(); // lock
            serverState -> matchIndex[peer_index] = serverState -> nextIndex[peer_index];
            serverState -> nextIndex[peer_index] ++;
            Update_CommitIndex(serverState, nodeInfo);
            serverState -> lck.unlock(); // unlock
        }
    }
    else {  /* rejected: the follower node lags behind */
        serverState -> lck.lock(); // lock
        serverState -> nextIndex[peer_index] --;
        serverState -> matchIndex[peer_index] = 0;
        serverState -> lck.unlock(); // unlock
    }


    return socket_status;

}

void ServerOutStub::Update_CommitIndex(ServerState *serverState, NodeInfo *nodeInfo) {
    int half_peers = nodeInfo -> num_peers / 2;
    int current_commitIndex = serverState -> commitIndex;
    int logSize = serverState -> smr_log.size();
    int nodeTerm = serverState -> currentTerm;

    int count;
    for (int N = current_commitIndex + 1; N < logSize; N++){
        count = 0;

        for (int j = 0; j < nodeInfo -> num_peers; j ++){
            if (serverState -> matchIndex[j] >= N){
                count++;
            }
        }

        if (count >= half_peers){

            /* Raft does not commit entry from previous term */
            if (serverState -> smr_log.at(N).logTerm == nodeTerm){
                serverState -> commitIndex = N;
            }
        }
    }
}








