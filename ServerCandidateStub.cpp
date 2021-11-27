#include "ServerCandidateStub.h"

/* return 0 on failure and 1 on success */
int ServerCandidateStub::Stub_SendRequestVote(ServerState *serverState, NodeInfo *nodeInfo, int fd) {
    RequestVote requestVote;
    int send_status;
    int size = requestVote.Size();
    char buf[size];

    FillRequestVote(serverState, nodeInfo, &requestVote);

    requestVote.Marshal(buf);
    send_status = Send_Message(buf, size, fd);

    return send_status;
}


void ServerCandidateStub::FillRequestVote(ServerState * serverState, NodeInfo * nodeInfo, RequestVote *requestVote) {
    int messageType = VOTE_REQUEST;
    int term = serverState -> currentTerm;
    int candidateId = nodeInfo -> node_id;
    int lastLogIndex = serverState -> smr_log.size() - 1;
    int lastLogTerm = serverState -> smr_log.back().logTerm;

    requestVote -> Set(messageType, term, candidateId, lastLogIndex, lastLogTerm);
}

/* functionalities include:
  ~ non-blocking receive VoteResponse
  ~ if receive heartbeat from an up-to-date leader, resign back to being a follower
*/
void ServerCandidateStub::
Stub_Handle_Poll_Candidate(std::vector<pollfd> *_pfds_server,
                           ServerState * serverState, std::map<int,int> *PeerIdIndexMap,
                           bool *request_completed, NodeInfo *nodeInfo){

    VoteResponse voteResponse;
    AppendEntryRequest appendEntryRequest;

    int peer_index;
    int messageType;
    int max_buf_size = voteResponse.Size() + appendEntryRequest.Size();
    char buf[max_buf_size];
    int nbytes;


    for(int i = 0; i < _pfds_server -> size(); i++) {   /* looping through file descriptors */
        if ( (*_pfds_server)[i].revents & POLLIN) {     /* got ready-to-read from poll() */

            if (i==0){                             /* events at the listening socket */
                Accept_Connection(_pfds_server);
            }

            else{                                   /* events from established connection */
                nbytes = recv( (*_pfds_server)[i].fd, buf, max_buf_size, 0);

                if (nbytes <= 0){   /* error handling for recv: remote connection closed or error */
                    close((*_pfds_server)[i].fd);
                    (*_pfds_server)[i].fd = -1;
                }

                else{    /* got good data */
                    memcpy(&messageType, buf, sizeof(messageType)); // get messageType

                    if (ntohl(messageType) == VOTE_RESPONSE) {

                        voteResponse.Unmarshal(buf);
                        voteResponse.Print();

                        peer_index = (*PeerIdIndexMap)[voteResponse.Get_node_id()];

                        // in case we get multiple response for the same request
                        if (!request_completed[peer_index]){

                            if (voteResponse.Get_voteGranted()) {
                                nodeInfo -> num_votes ++;
                            }

                            else {  // vote got rejected, which means the node lags behind
                                nodeInfo -> role = FOLLOWER;
                                serverState -> currentTerm = voteResponse.Get_term();
                            }
                            request_completed[peer_index] = true;
                        }
                    }

                    /*  when the candidate gets a log replication request from other node */
                    else if (ntohl(messageType) == APPEND_ENTRY_REQUEST) {

                        appendEntryRequest.UnMarshal(buf);

                        int leader_term = appendEntryRequest.Get_sender_term();
                        int localTerm = serverState -> currentTerm;
                        int leader_id = appendEntryRequest.Get_leaderId();

                        if (leader_term > localTerm) {
                            nodeInfo -> leader_id = leader_id;
                            nodeInfo -> role =  FOLLOWER;
                        }

                    }

                    else{
                        std::cout << "Undefined message type" << '\n';
                    }

                }                       /* End got good data */
            }                     /* End events from established connection */
        }                    /* End got ready-to-read from poll() */
    }                    /* End looping through file descriptors */
}