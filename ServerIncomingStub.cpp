#include "ServerIncomingStub.h"
#include <iostream>

ServerIncomingStub::ServerIncomingStub() {}

void ServerIncomingStub::Init(std::unique_ptr<ServerSocket> socket) {
	this->socket = std::move(socket);
}

/* return 0 on failure */
int ServerIncomingStub::Read_MessageType() {
    int net_messageType;
    int messageType;
    char buf[sizeof (int)];
    int socket_status;

    socket_status = socket -> Recv(buf, sizeof(int), 0);

    if (!socket_status){
        return 0;
    }

    memcpy(&net_messageType, buf, sizeof(net_messageType));
    messageType = ntohl(net_messageType);
    return messageType;
}

bool ServerIncomingStub::Send_MessageType(int messageType) {
    char buf[sizeof (int)];
    int socket_status;
    int net_messageType = htonl(messageType);

    memcpy(buf, &net_messageType, sizeof(net_messageType));
    socket_status = socket -> Send(buf, sizeof (int), 0);
    return socket_status;
}

/*------------------------Responding to Leader------------------------*/
bool ServerIncomingStub::
Handle_AppendEntryRequest(ServerState *serverState, std::mutex *lk_serverState) {
    ResponseAppendEntry responseAppendEntry;
    AppendEntryRequest appendEntryRequest;

    int success;
    int Heartbeat;
    char buf[sizeof (AppendEntryRequest)];
    bool socket_status;

    socket_status = socket -> Recv(buf, sizeof(AppendEntryRequest), 0);
    if (!socket_status){
        return socket_status;
    }

    appendEntryRequest.Unmarshal(buf);
    appendEntryRequest.Print();

    Send_MessageType(RESPONSE_APPEND_ENTRY);    // to-do: handle error gracefully here
    Heartbeat = (appendEntryRequest.Get_LogEntry().logTerm == - 1);

    lk_serverState -> lock();       // lock
    Set_Leader(&appendEntryRequest, serverState);
    Set_CommitIndex(&appendEntryRequest, serverState);
    success = Set_Result(serverState, &appendEntryRequest);
    responseAppendEntry.Set(serverState -> currentTerm, success, Heartbeat);
    lk_serverState -> unlock();     // unlock

    /* to-do: error checking send here */
    socket_status = Send_ResponseAppendEntry(&responseAppendEntry);
    return socket_status;
}


bool ServerIncomingStub::Send_ResponseAppendEntry(ResponseAppendEntry *responseAppendEntry){
    char buf[sizeof (ResponseAppendEntry)];
    bool socket_status;

    responseAppendEntry -> Marshal(buf);
    socket_status = socket -> Send(buf, sizeof (ResponseAppendEntry), 0);
    return socket_status;
}

void ServerIncomingStub::
Set_Leader(AppendEntryRequest *appendEntryRequest, ServerState *serverState){

    int remote_term = appendEntryRequest -> Get_sender_term();
    int localTerm = serverState -> currentTerm;

    if (remote_term >= localTerm) {
        serverState -> leader_id = appendEntryRequest -> Get_sender_term();
        serverState -> role =  FOLLOWER;
        serverState -> votedFor = -1;
        serverState -> currentTerm = remote_term;
    }
}

/* If leaderCommit > commitIndex,
 * Set commitIndex = min(leaderCommit, index of last new entry) */
void ServerIncomingStub::Set_CommitIndex(AppendEntryRequest *appendEntryRequest,
                                         ServerState * serverState) {

    /* from the remote side */
    int leaderCommit = appendEntryRequest -> Get_leaderCommit();

    /* local state */
    int local_commitIndex = serverState -> commitIndex;
    int local_log_length = serverState -> smr_log.size();

    if (leaderCommit > local_commitIndex){
        if (leaderCommit > local_log_length){
            serverState -> commitIndex = local_log_length;
        }
        else{
            serverState -> commitIndex = leaderCommit;
        }
    }
}

/* to-do: Clean this up */
bool ServerIncomingStub::Set_Result(ServerState *serverState,
                                    AppendEntryRequest *appendEntryRequest){

    /* from the remote side */
    int remote_term = appendEntryRequest -> Get_sender_term();
    int remote_prevLogIndex = appendEntryRequest -> Get_prevLogIndex();


    /* local state */
    int local_term = serverState -> currentTerm;
    int local_log_length = serverState -> smr_log.size();

    /* Reply false the remote node is stale */
    if (remote_term < local_term){
        return false;
    }

    /* heartbeat message */
    if (appendEntryRequest -> Get_LogEntry().logTerm == -1){
        return true;
    }

    else {  /* real log replication message */

        /* Reply false if log does not contain an entry at prevLogIndex whose term matches prevLogTerm */
        if (local_log_length - 1 < remote_prevLogIndex) {
            return false;
        }

        else {   // if there is an entry in the local log at remote_prevLogIndex
            return Check_ConflictingLog(serverState, appendEntryRequest);
        }
    }
}

/* Reply false if log does not contain an entry at prevLogIndex whose term
 * matches prevLogTerm */
bool ServerIncomingStub::Check_ConflictingLog(ServerState *serverState,
                                   AppendEntryRequest *appendEntryRequest){

    int local_prevLogTerm;
    int local_log_length = serverState -> smr_log.size();
    std::vector<LogEntry>::iterator iter = serverState -> smr_log.begin();

    int remote_prevLogIndex = appendEntryRequest -> Get_prevLogIndex();
    int remote_prevLogTerm = appendEntryRequest -> Get_prevLogTerm();
    LogEntry remote_logEntry = appendEntryRequest -> Get_LogEntry();

    local_prevLogTerm = serverState -> smr_log.at(remote_prevLogIndex).logTerm;

    /* if prev log entry does not match */
    if (local_prevLogTerm != remote_prevLogTerm) {

        if (local_log_length > 1) {
            /* erase conflicting log except the root log */
            serverState -> smr_log.erase(iter + remote_prevLogIndex,
                                         iter + local_log_length);
        }

        return false;
    }

    else {  /* if prev log entry matches */

        if (remote_logEntry.logTerm == serverState -> smr_log.back().logTerm) {
            return true;     // Already in the smr_log !
        }

        serverState -> smr_log.push_back(remote_logEntry);
        return true;
    }
}




void ServerIncomingStub::Print_Log(ServerState *serverState){
    LogEntry logEntry;
    int log_size = serverState -> smr_log.size();

    for (int i = 0; i < log_size; i++){
        logEntry = serverState -> smr_log.at(i);

        std::cout << "-----Log entry number: "<< i << "-----" << '\n';
        std::cout << "logTerm : "<< logEntry.logTerm << '\n';
    }
}





/*-----------------------Responding to Candidate--------------------------------*/
bool ServerIncomingStub::
Handle_VoteRequest(ServerState *serverState,  std::mutex *lk_serverState) {

    VoteRequest voteRequest;
    ResponseVote responseVote;
    char buf[voteRequest.Size()];
    int success;
    bool socket_status;

    socket_status = socket -> Recv(buf, sizeof(voteRequest), 0);

    if (!socket_status){
        return socket_status;
    }

    voteRequest.Unmarshal(buf);
    voteRequest.Print();

    lk_serverState -> lock();     // lock
    success = Decide_Vote(serverState, &voteRequest);
    responseVote.Set(serverState -> currentTerm, success);
    lk_serverState -> unlock();     // unlock

    Send_MessageType(RESPONSE_VOTE);
    socket_status = SendResponseVote(&responseVote);

    return socket_status;
}


bool ServerIncomingStub::SendResponseVote(ResponseVote *responseVote) {
    char buf[sizeof (ResponseVote)];
    bool socket_status;

    responseVote -> Marshal(buf);
    socket_status = socket -> Send(buf, sizeof (ResponseVote), 0);

    return socket_status;
}


bool ServerIncomingStub::
Decide_Vote(ServerState *serverState, VoteRequest *VoteRequest) {

    int result = false;
    int local_term = serverState -> currentTerm;
    int remote_term = VoteRequest -> Get_term();

    if (remote_term <= local_term){
        return false;
    }
    else {
        if (serverState -> role == LEADER){ // if stale leader receive vote request
            serverState -> role = FOLLOWER;
        }
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
bool ServerIncomingStub::Compare_Log(ServerState *serverState, VoteRequest * VoteRequest) {

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

/*----Client Interface---------*/

CustomerRequest ServerIncomingStub::ReceiveOrder() {
    char buffer[32];
    CustomerRequest order;

    if (socket -> Recv(buffer, order.Size(), 0)) {
        order.Unmarshal(buffer);
    }
    return order;
}

bool ServerIncomingStub::Send_LeaderID(int leaderID) {
    char buf[sizeof (int)];
    int socket_status;
    int net_leaderID = htonl(leaderID);

    memcpy(buf, &net_leaderID, sizeof(net_leaderID));
    socket_status = socket -> Send(buf, sizeof (int), 0);
    return socket_status;
}