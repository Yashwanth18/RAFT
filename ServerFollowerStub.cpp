#include "ServerFollowerStub.h"
#include <iostream>

ServerFollowerStub::ServerFollowerStub() {}

void ServerFollowerStub::Init(std::unique_ptr<ServerSocket> socket) {
	this->socket = std::move(socket);
}

/* return 0 on failure */
int ServerFollowerStub::Read_MessageType() {
    int net_messageType;
    int messageType;
    char buf[sizeof (int)];

    if (!socket -> Recv(buf, sizeof(int), 0)){
        perror("Read_MessageType"); // to-do: handle error gracefully here
        return 0;
    }

    memcpy(&net_messageType, buf, sizeof(net_messageType));
    messageType = ntohl(net_messageType);
    return messageType;
}

bool ServerFollowerStub::Send_MessageType(int messageType) {
    char buf[sizeof (int)];
    int send_status;
    int net_messageType = htonl(messageType);

    memcpy(buf, &net_messageType, sizeof(net_messageType));
    send_status = socket -> Send(buf, sizeof (int), 0);
    return send_status;
}

/*------------------------Responding to Leader------------------------*/
int ServerFollowerStub::
Handle_AppendEntryRequest(ServerState *serverState, NodeInfo *nodeInfo) {
    ResponseAppendEntry responseAppendEntry;
    AppendEntryRequest appendEntryRequest;

    int success;
    int Heartbeat;
    char buf[sizeof (AppendEntryRequest)];
    int send_status;

    if (!socket -> Recv(buf, sizeof(AppendEntryRequest), 0)){
        perror("Read_MessageType"); // to-do: handle error gracefully here
    }

    appendEntryRequest.Unmarshal(buf);
    appendEntryRequest.Print();

    Send_MessageType(RESPONSE_APPEND_ENTRY);    // to-do: handle error gracefully here

    Set_Leader(&appendEntryRequest, serverState, nodeInfo);
    Set_CommitIndex(&appendEntryRequest, serverState);
    success = Set_Result(serverState, &appendEntryRequest);


    Heartbeat = (appendEntryRequest.Get_LogEntry().logTerm == - 1);
    responseAppendEntry.Set(serverState -> currentTerm, success, Heartbeat);
    responseAppendEntry.Print();

    /* to-do: error checking send here */
    send_status = Send_ResponseAppendEntry(&responseAppendEntry);
    return send_status;
}


int ServerFollowerStub::Send_ResponseAppendEntry(ResponseAppendEntry *responseAppendEntry){
    char buf[sizeof (ResponseAppendEntry)];
    int send_status;

    responseAppendEntry -> Marshal(buf);
    send_status = socket -> Send(buf, sizeof (ResponseAppendEntry), 0);
    return send_status;
}


// ******* to-do: everytime nodeInfo or serverstate is accessed, make sure to use mutex lock!
void ServerFollowerStub::
Set_Leader(AppendEntryRequest *appendEntryRequest, ServerState *serverState, NodeInfo *nodeInfo){

    int remote_term = appendEntryRequest -> Get_sender_term();
    int localTerm = serverState -> currentTerm;

    if (remote_term >= localTerm) {
        nodeInfo -> leader_id = appendEntryRequest -> Get_sender_term();
        serverState -> role =  FOLLOWER;
        serverState -> votedFor = -1;
        serverState -> currentTerm = remote_term;
    }
}

/* If leaderCommit > commitIndex,
 * Set commitIndex = min(leaderCommit, index of last new entry) */
void ServerFollowerStub::Set_CommitIndex(AppendEntryRequest *appendEntryRequest, ServerState * serverState) {

    /* local state */
    int local_commitIndex = serverState -> commitIndex;
    int local_log_length = serverState -> smr_log.size();

    /* from the remote side */
    int leaderCommit = appendEntryRequest -> Get_leaderCommit();

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
bool ServerFollowerStub::Set_Result(ServerState *serverState, AppendEntryRequest *appendEntryRequest){
    /* local state */
    int local_term = serverState -> currentTerm;
    int local_log_length = serverState -> smr_log.size();
    int local_prevLogTerm;
    std::vector<LogEntry>::iterator iter = serverState -> smr_log.begin();

    /* from the remote side */
    int remote_term = appendEntryRequest -> Get_sender_term();
    int remote_prevLogIndex = appendEntryRequest -> Get_prevLogIndex();
    int remote_prevLogTerm = appendEntryRequest -> Get_prevLogTerm();
    LogEntry remote_logEntry = appendEntryRequest -> Get_LogEntry();

    /* Reply false the remote node is stale */
    if (remote_term < local_term){
        return false;
    }

    /* heartbeat message */
    if (appendEntryRequest -> Get_LogEntry().logTerm == -1){
        return true;
    }

    else {  // real log replication message

        /* Reply false if log does not contain an entry at prevLogIndex whose term matches prevLogTerm */
        if (local_log_length - 1 < remote_prevLogIndex) {
            return false; // no element in local smr_log
        }

        else {   // if there is an entry in the local log at remote_prevLogIndex
            local_prevLogTerm = serverState -> smr_log.at(remote_prevLogIndex).logTerm;

            if (local_prevLogTerm != remote_prevLogTerm) { // check if conflicting prev log entry

                if (local_log_length > 1) {  // keep first log to prevent index out of bound error
                    /* erase conflicting log */
                    serverState -> smr_log.erase(iter + remote_prevLogIndex, iter + local_log_length);
                }

                return false;
            }

            else {
                // check for conflicting entry at the last index of local smr_log
                if (remote_logEntry.logTerm == serverState -> smr_log.back().logTerm) {
                    return true;     // Already in the smr_log !
                }

                serverState->smr_log.push_back(remote_logEntry);
                return true;
            }
        }
    }
}


void ServerFollowerStub::Print_Log(ServerState *serverState){
    LogEntry logEntry;
    int log_size = serverState -> smr_log.size();

    for (int i = 0; i < log_size; i++){
        logEntry = serverState -> smr_log.at(i);

        std::cout << "-----Log entry number: "<< i << "-----" << '\n';
        std::cout << "logTerm : "<< logEntry.logTerm << '\n';
    }
}





/*-----------------------Responding to Candidate--------------------------------*/
int ServerFollowerStub::
Handle_VoteRequest(ServerState *serverState, NodeInfo *nodeInfo) {

    VoteRequest voteRequest;
    ResponseVote ResponseVote;
    char buf[voteRequest.Size()];
    int success;
    int send_status;
    if (!socket -> Recv(buf, sizeof(voteRequest), 0)){
      perror("Read_MessageType");
      return 0;
    }
    voteRequest.Unmarshal(buf);
    voteRequest.Print();

    success = Decide_Vote(serverState, &voteRequest);
    ResponseVote.Set(serverState -> currentTerm, success);
    Send_MessageType(RESPONSE_VOTE);
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
