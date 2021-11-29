#include "ServerFollowerStub.h"

/* functionalities include: VoteRequestRPC & AppendEntryRPC */
void ServerFollowerStub::
Stub_Handle_Poll_Follower(ServerTimer *timer, std::vector<pollfd> *_pfds_server,
                          ServerState *serverState, NodeInfo *nodeInfo){

    int max_data_size = sizeof(AppendEntryRequest) + sizeof(ResponseAppendEntry) +
                        sizeof(VoteRequest) + sizeof(ResponseVote);
    char buf[max_data_size];
    int messageType;
    int read_status;

    for(int i = 0; i < _pfds_server -> size(); i++) {   /* looping through file descriptors */
        pollfd pfd = (*_pfds_server)[i];
        if (pfd.revents & POLLIN) {            /* got ready-to-read from poll() */

            if (i==0){ /* events at the listening socket */
                Accept_Connection(_pfds_server);
            }

            else { /* events from established connection */
                read_status = Read_Message(pfd.fd, buf, max_data_size); /* to-do: implement proper read all bytes*/

                if (read_status <= 0){  /* connection closed or error */
                    close(pfd.fd);
                    pfd.fd = -1;     // never delete
                }

                else{   /* got good data */
                    messageType = Unmarshal_MessageType(buf);
                    if (messageType == APPEND_ENTRY_REQUEST){   // main functionality
                        std::cout << "Follower received AppendEntryRequest" << '\n';
                        Handle_AppendEntryRequest(serverState, nodeInfo, buf, pfd.fd);
                    }
                    else if (messageType == VOTE_REQUEST){      // main functionality
                        std::cout << "Follower received VoteRequest" << '\n';
                        Handle_VoteRequest(serverState, nodeInfo, buf, pfd.fd);
                    }
                    else if (messageType == RESPONSE_VOTE){
                        std::cout << "Follower received ResponseVote" << '\n';
                        // do nothing here?
                    }
                    else if (messageType == RESPONSE_APPEND_ENTRY){
                        std::cout << "Follower received ResponseAppendEntry" << '\n';
                        // do nothing here?
                    }
                    else{
                        std::cout << "Follower received undefined message type" << '\n';
                        std::cout << "read_status: " << read_status << '\n';
                        std::cout << "messageType: " << messageType << '\n';
                    }
                    timer -> Restart();
                } /* End got good data */
            } /* End events from established connection */
        } /* End: got ready-to-read from poll() */
    } /*  End: looping through file descriptors */
}

void ServerFollowerStub::
Set_Leader(AppendEntryRequest *appendEntryRequest, ServerState *serverState, NodeInfo *nodeInfo){

    int remote_term = appendEntryRequest -> Get_term();
    int localTerm = serverState -> currentTerm;

    if (remote_term >= localTerm) {
        nodeInfo -> leader_id = appendEntryRequest -> Get_term();
        nodeInfo -> role =  FOLLOWER;
        serverState -> votedFor = -1;
        serverState -> currentTerm = remote_term;
    }
}

void ServerFollowerStub::
Handle_AppendEntryRequest(ServerState *serverState, NodeInfo *nodeInfo, char *buf, int fd) {

    AppendEntryRequest appendEntryRequest;
    ResponseAppendEntry ResponseAppendEntry;
    int success;
    int ResponseID;

    appendEntryRequest.Unmarshal(buf);
    appendEntryRequest.Print();

    Set_Leader(&appendEntryRequest, serverState, nodeInfo);
    Set_CommitIndex(&appendEntryRequest, serverState);

    success = Set_Result(serverState, &appendEntryRequest);

    ResponseID = appendEntryRequest.Get_RequestID() + 1;
    ResponseAppendEntry.Set(RESPONSE_APPEND_ENTRY, serverState -> currentTerm,
                            success, nodeInfo -> node_id, ResponseID);

    Send_ResponseAppendEntry(&ResponseAppendEntry, fd); /* do error checking send here? */
}


void ServerFollowerStub::
Handle_VoteRequest(ServerState *serverState, NodeInfo *nodeInfo, char *buf, int fd) {

    VoteRequest voteRequest;
    ResponseVote ResponseVote;
    int success;

    voteRequest.Unmarshal(buf);
    voteRequest.Print();

    success = Decide_Vote(serverState, nodeInfo, &voteRequest);

    ResponseVote.Set(RESPONSE_VOTE, serverState -> currentTerm, success, nodeInfo -> node_id);

    SendResponseVote(&ResponseVote, fd); /* do error checking send here? */
}


/* --------------------------- Election Module helper functions--------------------------------------------- */

/* return 0 on failure and 1 on success */
int ServerFollowerStub::SendResponseVote(ResponseVote *ResponseVote, int fd) {
    int size = ResponseVote -> Size();
    char buf[size];
    ResponseVote -> Marshal(buf);
    return Send_Message(buf, size, fd);
}

int ServerFollowerStub::Decide_Vote(ServerState *serverState, NodeInfo *nodeInfo, VoteRequest *VoteRequest) {
    int result = false;
    int local_term = serverState -> currentTerm;
    int remote_term = VoteRequest -> Get_term();

    if (remote_term > local_term){
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


/* ------------------------------------------------------------------------------------------------------------- */
/* ------------------------------  Log Replication helper functions------------------------------------------- */

int ServerFollowerStub::Send_ResponseAppendEntry(ResponseAppendEntry *ResponseAppendEntry, int fd){
    int size = ResponseAppendEntry -> Size();
    char buf[size];
    int send_status;

    ResponseAppendEntry -> Marshal(buf);
    send_status = Send_Message(buf, size, fd);
    return send_status;
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
    int remote_term = appendEntryRequest -> Get_term();
    int remote_prevLogIndex = appendEntryRequest -> Get_prevLogIndex();
    int remote_prevLogTerm = appendEntryRequest -> Get_prevLogTerm();
    LogEntry remote_logEntry = appendEntryRequest -> Get_LogEntry();

    /* Reply false the remote node is stale */
    if (remote_term < local_term){
        return false;
    }

    if (appendEntryRequest -> Get_RequestID() == -1){      // heartbeat message
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
    for (int i = 0; i < serverState -> smr_log.size(); i++){
        logEntry = serverState -> smr_log.at(i);

        std::cout << "-----Log entry number: "<< i << "-----" << '\n';
        std::cout << "logTerm : "<< logEntry.logTerm << '\n';
    }
}