#include "ServerCandidateStub.h"

/* return 0 on failure and 1 on success */
int ServerCandidateStub::Stub_SendVoteRequest(ServerState *serverState, NodeInfo *nodeInfo, int fd) {
    VoteRequest VoteRequest;
    int send_status;
    int size = VoteRequest.Size();
    char buf[size];

    FillVoteRequest(serverState, nodeInfo, &VoteRequest);

    VoteRequest.Marshal(buf);
    send_status = Send_Message(buf, size, fd);

    return send_status;
}


void ServerCandidateStub::FillVoteRequest(ServerState * serverState, NodeInfo * nodeInfo, VoteRequest *VoteRequest) {
    int messageType = VOTE_REQUEST;
    int term = serverState -> currentTerm;
    int candidateId = nodeInfo -> node_id;
    int lastLogIndex = serverState -> smr_log.size() - 1;
    int lastLogTerm = serverState -> smr_log.back().logTerm;

    VoteRequest -> Set(messageType, term, candidateId, lastLogIndex, lastLogTerm);
}

/* functionalities include:
  ~ non-blocking receive ResponseVote
  ~ if receive heartbeat from an up-to-date leader, resign back to being a follower
*/
void ServerCandidateStub::
Stub_Handle_Poll_Candidate(std::vector<pollfd> *_pfds_server,
                           ServerState * serverState, std::map<int,int> *PeerIdIndexMap,
                           bool *Request_completed, NodeInfo *nodeInfo){


    int messageType;
    ResponseVote ResponseVote;
    AppendEntryRequest appendEntryRequest;

    int max_buf_size = ResponseVote.Size() + appendEntryRequest.Size();
    std::cout << "candidate poll: max_buf_size" << max_buf_size << '\n';
    char buf[max_buf_size];
    int nbytes;
    pollfd pfd;

    for(int i = 0; i < _pfds_server -> size(); i++) {   /* looping through file descriptors */
        pfd = (*_pfds_server)[i];

        if ( pfd.revents & POLLIN) {     /* got ready-to-read from poll() */

            if (i==0){                             /* events at the listening socket */
                Accept_Connection(_pfds_server);
            }

            else{                                   /* events from established connection */
                nbytes = recv( pfd.fd, buf, max_buf_size, 0);

                if (nbytes <= 0){   /* error handling for recv: remote connection closed or error */
                    close(pfd.fd);
                    pfd.fd = -1;
                }

                else{    /* got good data */
                    messageType = Unmarshal_MessageType(buf);

                    /*  when the candidate gets a log replication request from a leader node */
                    if (messageType == APPEND_ENTRY_REQUEST) {  // main functionality
                        std::cout << " Candidate received AppendEntryRequest" << '\n';
                        Handle_AppendEntryRequest(nodeInfo, serverState, buf);
                    }
                    else if (messageType == VOTE_REQUEST){
                        std::cout << " Candidate received VoteRequest" << '\n';
                        // do nothing here?
                    }

                    else if (messageType == RESPONSE_VOTE) {     // main functionality
                        std::cout << " Candidate received ResponseVote" << '\n';
                        Handle_ResponseVote(nodeInfo, serverState, buf, Request_completed, PeerIdIndexMap);
                    }

                    else if (messageType == RESPONSE_APPEND_ENTRY){
                        std::cout << " Candidate received ResponseAppendEntry" << '\n';
                        // do nothing here?
                    }

                    else{
                        std::cout << "Candidate received undefined message type" << '\n';
                    }

                }    /* End got good data */
            }  /* End events from established connection */
        }   /* End got ready-to-read from poll() */
    }    /* End looping through file descriptors */
}


void ServerCandidateStub::
Handle_ResponseVote(NodeInfo *nodeInfo, ServerState *serverState, char *buf,
                    bool *Request_completed, std::map<int,int> *PeerIdIndexMap){

    ResponseVote ResponseVote;
    int peer_index;
    
    ResponseVote.Unmarshal(buf);
    ResponseVote.Print();

    peer_index = (*PeerIdIndexMap)[ResponseVote.Get_nodeID()];

    // in case we get multiple response for the same request
    if (!Request_completed[peer_index]){

        if (ResponseVote.Get_voteGranted()) { // vote granted
            nodeInfo -> num_votes ++;
        }

        else {  // vote got rejected
            if (ResponseVote.Get_term() > serverState -> currentTerm){ // we are stale
                nodeInfo -> role = FOLLOWER;
                serverState -> currentTerm = ResponseVote.Get_term();
            }
        }

        Request_completed[peer_index] = true;
    }
}

void ServerCandidateStub::
Handle_AppendEntryRequest(NodeInfo *nodeInfo, ServerState *serverState, char *buf) {
    AppendEntryRequest appendEntryRequest;
    appendEntryRequest.Unmarshal(buf);

    int remote_term = appendEntryRequest.Get_term();
    int localTerm = serverState -> currentTerm;

    if (remote_term >= localTerm) {
        nodeInfo -> leader_id = appendEntryRequest.Get_nodeID();
        nodeInfo -> role =  FOLLOWER;
        serverState -> votedFor = -1;
        serverState -> currentTerm = remote_term;
    }
}