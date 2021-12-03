#include "ServerOutStub.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include "ServerConfiguration.h"
#include <fstream>

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
Handle_ResponseVote(ServerState *serverState, std::mutex *lk_serverState){

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
        lk_serverState -> lock();
        serverState -> num_votes ++;
        lk_serverState -> unlock();
    }

    else {  /* vote got rejected */
        if (responseVote.Get_term() > serverState -> currentTerm){ // we are stale
            lk_serverState->lock();
            serverState -> currentTerm = responseVote.Get_term();
            lk_serverState -> unlock();
        }
    }

    return socket_status;
}

/*--------------------------Leader Helper Functions---------------------------- */


bool ServerOutStub::
SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo, int peer_index, int heartbeat) {
    AppendEntryRequest appendEntryRequest;
    char buf[sizeof(AppendEntryRequest)];
    bool socket_status;

    FillAppendEntryRequest(serverState, nodeInfo, &appendEntryRequest, peer_index, heartbeat);

    appendEntryRequest.Marshal(buf);
    socket_status = socket.Send(buf, sizeof(AppendEntryRequest), 0);

    return socket_status;
}

void ServerOutStub::FillAppendEntryRequest(ServerState * serverState, NodeInfo * nodeInfo,
                                              AppendEntryRequest *appendEntryRequest,
                                              int peer_index, int heartbeat) {

    int sender_term = serverState -> currentTerm;
    int leaderId = nodeInfo -> node_id;
    int leaderCommit = serverState -> commitIndex;

    int prevLogIndex = serverState -> nextIndex[peer_index] - 1;
    int prevLogTerm = -1;
    LogEntry logEntry;

    int last_log_index = serverState -> smr_log.size() -1;
    int nextIndexPeer = serverState -> nextIndex[peer_index];

    /* to-do: break this into smaller functions: Set_LogEntry() or something*/
    if (heartbeat){           // heartbeat
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
                              &logEntry, leaderCommit);
}

bool ServerOutStub::Handle_ResponseAppendEntry(ServerState *serverState, int peer_index) {
    char buf[sizeof (ResponseAppendEntry)];
    ResponseAppendEntry responseAppendEntry;
    int remote_term;
    int local_term;
    bool socket_status;

    socket_status = socket.Recv(buf, sizeof(ResponseAppendEntry), 0);

    if (!socket_status){
        perror("Handle_ResponseAppendEntry: Recv"); // to-do: handle error gracefully here
        return false;
    }

    responseAppendEntry.Unmarshal(buf);
    remote_term = responseAppendEntry.Get_term();

    local_term = serverState -> currentTerm;

    if (remote_term > local_term){   // check if we are stale
        std::cout << "\nremote_term: " << remote_term << '\n';
        std::cout << "local_term: " << local_term << '\n';
        std::cout << "Handle_ResponseEntry: Leader Resigning to be a follower "<< '\n';

        serverState -> role = FOLLOWER;
        serverState -> currentTerm = responseAppendEntry.Get_term();
    }

    if (responseAppendEntry.Get_Heartbeat()){
        responseAppendEntry.Print();
    }

    else{   // if response from proper log replication request
        responseAppendEntry.Print();

        if (responseAppendEntry.Get_success()) {
            serverState -> nextIndex[peer_index] ++;
            // set match index
            // set commit index based on the majority of matchIndex[]

          Write_ServerLogToAStorage(serverState);
          return true;
        }

        else {  /* rejected: the follower node lags behind */
            serverState -> nextIndex[peer_index] --;
        }
    }

    return socket_status;

}

void ServerOutStub::Write_ServerLogToAStorage(ServerState *server_state) {
  if(configMap.count(server_state -> nodeId)){
    std::vector<std::string> vector_file  = configMap.at(server_state -> nodeId);
    std::string log_smr_file = vector_file.front();
    std::ofstream write_smr_log (log_smr_file, std::ios::app);
    LogEntry committingLogEntry = server_state -> smr_log[server_state -> commitIndex -1 ];
    write_smr_log << committingLogEntry.logTerm << "," << committingLogEntry.opcode <<
                  committingLogEntry.arg1 << "," << committingLogEntry.arg1;
    write_smr_log.close();
  }
}






