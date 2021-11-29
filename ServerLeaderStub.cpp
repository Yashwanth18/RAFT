#include "ServerLeaderStub.h"

/* functionalities include: Receive acknowledgement from Follower */
void ServerLeaderStub::
Stub_Handle_Poll_Leader(std::vector<pollfd> *_pfds_server, NodeInfo *nodeInfo, ServerState *serverState,
                   std::map<int,int> *PeerIdIndexMap, int * RequestID){

    int messageType;
    int nbytes;
    int max_data_size = sizeof(AppendEntryRequest) + sizeof(ResponseAppendEntry) +
                        sizeof(VoteRequest) + sizeof(ResponseVote);
    char buf[max_data_size];
    int num_alive_sockets = _pfds_server -> size();
    pollfd pfd;

    for(int i = 0; i < num_alive_sockets; i++) {   /* looping through file descriptors */
        pfd = (*_pfds_server)[i];

        if (pfd.revents & POLLIN) {     /* got ready-to-read from poll() */
            if (i==0){                             /* events at the listening socket */
                Accept_Connection(_pfds_server);
            }

            else{                                   /* events from established connection */
                nbytes = recv(pfd.fd, buf, max_data_size, 0);

                if (nbytes <= 0){   /* error handling for recv: remote connection closed or error */
                    close(pfd.fd);
                    pfd.fd = -1;
                }

                else{    /* got good data */
                    messageType = Unmarshal_MessageType(buf);

                    if (messageType == APPEND_ENTRY_REQUEST){
                        std::cout << "Leader received appendEntryRequest" << '\n';
                        Handle_AppendEntryRequest(serverState, nodeInfo, buf);
                    }
                    else if (messageType == VOTE_REQUEST){
                        std::cout << "Leader received VoteRequest" << '\n';
                        Handle_VoteRequest(serverState, nodeInfo, buf);
                    }
                    else if (messageType == RESPONSE_VOTE){
                        std::cout << "Leader received ResponseVote" << '\n';
                        Handle_ResponseVote(serverState, nodeInfo, buf);
                    }

                    else if (messageType == RESPONSE_APPEND_ENTRY){     // the main functionality
                        std::cout << "Leader received ResponseAppendEntry" << '\n';
                        Handle_ResponseAppendEntry(buf, nodeInfo, serverState, PeerIdIndexMap, RequestID, i);
                    }

                    else{
                        std::cout << " Leader received undefined message type" << '\n';
                    }
                } /* End got good data */
            } /* End events from established connection */
        } /* End got ready-to-read from poll() */
    } /* End looping through file descriptors */
}

void ServerLeaderStub::Handle_ResponseAppendEntry(char *buf, NodeInfo *nodeInfo,
                                                  ServerState *serverState, std::map<int, int> *PeerIdIndexMap,
                                                  int *RequestID, int socket_index) {
    ResponseAppendEntry responseAppendEntry;
    responseAppendEntry.Unmarshal(buf);
    responseAppendEntry.Print();
    int peer_index;

    if (responseAppendEntry.Get_term() < serverState -> currentTerm){   // check if we are stale
        nodeInfo -> role = FOLLOWER;
        serverState -> currentTerm = responseAppendEntry.Get_term();
    }

    if (responseAppendEntry.Get_ResponseID() == HEARTBEAT){ // HEARTBEAT defined as 0
        std::cout << "Leader received heartbeat ack "<< '\n';
    }

    else{   // if response from proper log replication request
        peer_index = (*PeerIdIndexMap)[responseAppendEntry.Get_nodeID()];

        if (responseAppendEntry.Get_success()) {
            serverState -> nextIndex[peer_index] ++;
        }

        else {  // rejected: the follower node lags behind
            if (responseAppendEntry.Get_ResponseID() > RequestID[socket_index]){    // to handle duplicate message
                serverState -> nextIndex[peer_index] --;
            }
        }
        RequestID[socket_index]++;  // to handle duplicate message
    }
}

/* Case where there is a stale leader */
void ServerLeaderStub::
Handle_AppendEntryRequest(ServerState *serverState, NodeInfo *nodeInfo, char *buf) {

    AppendEntryRequest appendEntryRequest;
    appendEntryRequest.Unmarshal(buf);
    int remote_term = appendEntryRequest.Get_term();
    int local_term = serverState -> currentTerm;

    if (remote_term > local_term){
        nodeInfo -> leader_id = appendEntryRequest.Get_nodeID();
        nodeInfo -> role =  FOLLOWER;
        serverState -> votedFor = -1;
        serverState -> currentTerm = remote_term;
    }
}

void ServerLeaderStub::
Handle_ResponseVote(ServerState *serverState, NodeInfo *nodeInfo, char *buf) {
    ResponseVote responseVote;

    responseVote.Unmarshal(buf);
    int remote_term = responseVote.Get_term();
    int local_term = serverState -> currentTerm;

    std::cout << "remote_term: " << remote_term << '\n';
    std::cout << "local_term: " << local_term << '\n';

    if (remote_term > local_term){
        nodeInfo -> role = FOLLOWER;
        serverState -> currentTerm = remote_term;
    }
}

void ServerLeaderStub::
Handle_VoteRequest(ServerState *serverState, NodeInfo *nodeInfo, char *buf) {
    VoteRequest VoteRequest;

    VoteRequest.Unmarshal(buf);
    int remote_term = VoteRequest.Get_term();
    int local_term = serverState -> currentTerm;

    if (remote_term > local_term){
        nodeInfo -> role = FOLLOWER;
        serverState -> currentTerm = remote_term;
    }
}


/* --------------------------------Log Replication helper functions--------------------------------------------- */
int ServerLeaderStub::
Stub_SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo, int fd, int peer_index, int RequestID) {
    AppendEntryRequest appendEntryRequest;
    int size = appendEntryRequest.Size();
    char buf[size];

    FillAppendEntryRequest(serverState, nodeInfo, &appendEntryRequest, peer_index, RequestID);

    appendEntryRequest.Marshal(buf);
    return Send_Message(buf, size, fd);
}

void ServerLeaderStub::FillAppendEntryRequest(ServerState * serverState, NodeInfo * nodeInfo,
                       AppendEntryRequest *appendEntryRequest,  int peer_index, int RequestID) {

    int _messageType = APPEND_ENTRY_REQUEST;
    int _sender_term = serverState -> currentTerm;
    int _leaderId = nodeInfo -> node_id;
    int _leaderCommit = serverState -> commitIndex;

    int _prevLogIndex = serverState -> nextIndex[peer_index] - 1;
    int _prevLogTerm = -1;
    LogEntry logEntry;

    int last_log_index = serverState -> smr_log.size() -1;
    int nextIndexPeer = serverState -> nextIndex[peer_index];
    std::cout << "nextIndexPeer: " << nextIndexPeer << '\n';

    if (nextIndexPeer > last_log_index){
        perror("nextIndexPeer out of range");
    }

    if (RequestID == -1){
        logEntry = LogEntry{-1, -1, -1, -1};
    }
    else{
        logEntry = serverState -> smr_log.at(nextIndexPeer);
    }

    if (_prevLogIndex >= 0){
        _prevLogTerm = serverState -> smr_log.at(_prevLogIndex).logTerm;
    }

    appendEntryRequest -> Set(_messageType, _sender_term, _leaderId, _prevLogTerm, _prevLogIndex,
                              &logEntry, _leaderCommit, RequestID);
}

