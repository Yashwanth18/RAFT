#include "ServerLeaderStub.h"

/* ---------------------------------Leader helper function---------------------------------- */
/* functionalities include: Receive acknowledgement from Follower */
void ServerLeaderStub::
Stub_Handle_Poll_Leader(std::vector<pollfd> *_pfds_server, ServerState *serverState,
                   std::map<int,int> *PeerIdIndexMap, int * RequestID){

    ResponseAppendEntry ResponseAppendEntry;

    char buf[ResponseAppendEntry.Size()];
    int num_alive_sockets = _pfds_server -> size();
    int peer_index;

    for(int i = 0; i < num_alive_sockets; i++) {   /* looping through file descriptors */
        if ((*_pfds_server)[i].revents & POLLIN) {     /* got ready-to-read from poll() */

            if (i==0){                             /* events at the listening socket */
                Accept_Connection(_pfds_server);
            }

            else{                                   /* events from established connection */
                int nbytes = recv((*_pfds_server)[i].fd, buf, sizeof(ResponseAppendEntry), 0);

                if (nbytes <= 0){   /* error handling for recv: remote connection closed or error */
                    close((*_pfds_server)[i].fd);
                    (*_pfds_server)[i].fd = -1;
                }

                else{    /* got good data */
                    ResponseAppendEntry.UnMarshal(buf);
                    ResponseAppendEntry.Print();

                    if (ResponseAppendEntry.Get_messageType() != APPEND_ENTRY_RESPONSE){
                        perror( "Undefined Message error type received by leader" );
                    }

                    if (ResponseAppendEntry.Get_ResponseID() == 0){ // if heartbeat
                        // to-do: check if the leader term is stale here
                        std::cout << "Received heartbeat ack "<< '\n';
                    }

                    else{   // if response from proper log replication request
                        peer_index = (*PeerIdIndexMap)[ResponseAppendEntry.Get_nodeID()];

                        if (ResponseAppendEntry.Get_success()) {
                            serverState -> nextIndex[peer_index] ++;
                        }

                        else {  // rejected: the follower node lags behind /* to-do */
                            if (ResponseAppendEntry.Get_ResponseID() > RequestID[i]){
                                serverState -> nextIndex[peer_index] --;
                                RequestID[i]++;
                            }
                        }
                    }


                } /* End got good data */
            } /* End events from established connection */
        } /* End got ready-to-read from poll() */
    } /* End looping through file descriptors */
}

/* ---------Leader: Log Replication helper functions--------------------------------------------- */
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
    int last_log_index = serverState -> smr_log.size() -1;

    int nextIndexPeer = serverState -> nextIndex[peer_index];
    int _prevLogIndex = serverState -> nextIndex[peer_index] - 1;
    int _prevLogTerm = -1;

    std::cout << "nextIndexPeer: " << nextIndexPeer << '\n';

    if (nextIndexPeer > last_log_index){
        perror("nextIndexPeer out of range");
    }

    LogEntry logEntry = serverState -> smr_log.at(nextIndexPeer);

    if (_prevLogIndex >= 0){
        _prevLogTerm = serverState -> smr_log.at(_prevLogIndex).logTerm;
    }

    appendEntryRequest -> Set(_messageType, _sender_term, _leaderId, _prevLogTerm, _prevLogIndex,
                              &logEntry, _leaderCommit, RequestID);
}

