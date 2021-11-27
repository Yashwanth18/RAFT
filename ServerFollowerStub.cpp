#include "ServerFollowerStub.h"
/* ---------------------------------Follower Helper function: ---------------------------------- */

/* functionalities include: RequestVoteRPC & AppendEntryRPC */
void ServerFollowerStub::
Stub_Handle_Poll_Follower(std::vector<pollfd> *_pfds_server, ServerState *serverState, NodeInfo *nodeInfo){
    AppendEntryRequest appendEntryRequest;
    ResponseAppendEntry ResponseAppendEntry;
    RequestVote requestVote;
    VoteResponse voteResponse;

    int max_buf_size = appendEntryRequest.Size() + requestVote.Size();
    char buf[max_buf_size];
    int success;
    int net_messageType;
    int messageType;
    int ResponseID;

    for(int i = 0; i < _pfds_server -> size(); i++) {   /* looping through file descriptors */
        if ((*_pfds_server)[i].revents & POLLIN) {            /* got ready-to-read from poll() */

            if (i==0){ /* events at the listening socket */
                Accept_Connection(_pfds_server);
            }

            else { /* events from established connection */

                int nbytes = recv((*_pfds_server)[i].fd, buf, max_buf_size, 0);
                memcpy(&net_messageType, buf, sizeof(net_messageType));
                messageType = ntohl(net_messageType);

                if (nbytes <= 0){  /* connection closed or error */
                    close((*_pfds_server)[i].fd);
                    (*_pfds_server)[i].fd = -1;     // never delete
                }
                else{             /* got good data */

                    if (messageType == APPEND_ENTRY_REQUEST){
                        appendEntryRequest.UnMarshal(buf);
                        appendEntryRequest.Print();

                        Set_CommitIndex(&appendEntryRequest, serverState);
                        success = Set_Result(serverState, &appendEntryRequest);

                        ResponseID = appendEntryRequest.Get_RequestID() + 1;
                        ResponseAppendEntry.Set(APPEND_ENTRY_RESPONSE, serverState -> currentTerm,
                                                success, nodeInfo -> node_id, ResponseID);

                        Send_ResponseAppendEntry(&ResponseAppendEntry, (*_pfds_server)[i].fd);
                    }
                    else if (messageType == VOTE_REQUEST){
                        requestVote.Unmarshal(buf);
                        requestVote.Print();

                        success = Decide_Vote(serverState, nodeInfo, &requestVote);

                        voteResponse.Set(VOTE_RESPONSE, serverState -> currentTerm, success,
                                         nodeInfo -> node_id);

                        SendVoteResponse(&voteResponse, (*_pfds_server)[i].fd);
                    }
                } /* End got good data */
            } /* End events from established connection */
        } /* End: got ready-to-read from poll() */
    } /*  End: looping through file descriptors */
}


/* --------- Election Module--------------------------------------------- */

/* return 0 on failure and 1 on success */
int ServerFollowerStub::SendVoteResponse(VoteResponse *voteResponse, int fd) {
    int size = voteResponse -> Size();
    char buf[size];
    voteResponse -> Marshal(buf);
    return Send_Message(buf, size, fd);
}

int ServerFollowerStub::Decide_Vote(ServerState *serverState, NodeInfo *nodeInfo, RequestVote *requestVote) {
    int result = false;
    int local_term = serverState -> currentTerm;
    int remote_term = requestVote -> Get_term();

    if (Compare_Log (serverState, nodeInfo, requestVote) && serverState -> votedFor == -1){
        result = (remote_term > local_term);
    }

    if (result){
        serverState -> votedFor = requestVote -> Get_candidateId();
        serverState -> currentTerm = requestVote -> Get_term();
    }

    return result;
}

/* Comparing the last_term and log length for the candidate node and the follower node */
int ServerFollowerStub::Compare_Log(ServerState *serverState, NodeInfo * nodeInfo, RequestVote * requestVote) {

    int candidate_last_log_term = requestVote -> Get_last_log_term();
    int candidate_last_log_index = requestVote -> Get_last_log_index();

    int local_last_log_term = serverState -> smr_log.back().logTerm;
    int local_last_log_index = serverState -> smr_log.size() -1;

    int greater_last_log_term = candidate_last_log_term > local_last_log_term;
    int check_last_log_index = candidate_last_log_index >= local_last_log_index;

    int result = false;

    if (greater_last_log_term){
        result = true;
    }

    else if (candidate_last_log_term == local_last_log_term){
        return check_last_log_index;
    }
    return result;
}

/* ------------  Log Replication helper functions--------------------------- */


int ServerFollowerStub::Send_ResponseAppendEntry(ResponseAppendEntry *ResponseAppendEntry, int fd){
    int size = ResponseAppendEntry -> Size();
    char buf[size];

    ResponseAppendEntry -> Marshal(buf);

    return Send_Message(buf, size, fd);
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



/* to-do: Ideally, should return false to the leader first before modifying local log to
 * optimize latency */
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