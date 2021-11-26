#include"ServerStub.h"

void ServerStub:: Init(NodeInfo * nodeInfo){
  Add_Socket_To_Poll(ListenSocket.Init(nodeInfo -> server_port));  /* Listen to peer servers  */
}

void ServerStub:: Add_Socket_To_Poll(int new_fd){
    pollfd new_pfd;
    new_pfd.fd = new_fd;
    new_pfd.events = POLLIN;
    pfds_server.push_back(new_pfd);
}


int ServerStub::Create_Socket() {
    int new_fd;
    new_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (new_fd < 0) {
        perror("ERROR: failed to create a socket");
        return -1;
    }

    return new_fd;
}

/* return 0 on failure and 1 on success */
int ServerStub:: Connect_To(std::string ip, int port, int new_fd){
    struct sockaddr_in addr;
    int connect_status;

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);


    try{
        connect_status = connect(new_fd, (struct sockaddr *) &addr, sizeof(addr));
        if (connect_status < 0){
            throw connect_status;
        }
    }
    catch(int stat){
        return 0;
    }

    fcntl(new_fd, F_SETFL, O_NONBLOCK);   /* set socket to non-blocking */
    return 1;
}

void ServerStub:: Accept_Connection(){
  int new_fd;
  struct sockaddr_in addr;
  unsigned int addr_size = sizeof(addr);

  // the listening socket is pfds_server[0].fd
  new_fd = accept(pfds_server[0].fd, (struct sockaddr *) &addr, &addr_size);
  if (new_fd < 0) perror ("accept");

  Add_Socket_To_Poll(new_fd);
}

int ServerStub:: Poll(int poll_timeout){
    int poll_count = poll(pfds_server.data(), pfds_server.size(), poll_timeout);
    if( poll_count < 0 )   perror("poll");
    return poll_count;
}

/* -------------------------------Candidate Helper Functions-----------------------------------------*/

/* functionalities include:
  ~ non-blocking receive VoteResponse
*/
void ServerStub:: Handle_Poll_Candidate(ServerState * serverState, std::map<int,int> *PeerIdIndexMap,
                                   bool *request_completed,
                                   NodeInfo *nodeInfo){
    VoteResponse voteResponse;
    AppendEntryRequest appendEntryRequest;
    int max_buf_size = voteResponse.Size() + appendEntryRequest.Size();

    char buf[max_buf_size];
    int num_peers = nodeInfo -> num_peers;
    int peer_index;
    /* message descriptor to determine if the message is appendEntryRequest or vote response */
    int message_type;

    for(int i = 0; i < num_peers; i++) {   /* looping through file descriptors */
        if (pfds_server[i].revents & POLLIN) {     /* got ready-to-read from poll() */

          std::cout << "readable data is ready " << '\n';

            if (i==0){                             /* events at the listening socket */
                Accept_Connection();
            }

            else{                                   /* events from established connection */
                int nbytes = recv(pfds_server[i].fd, buf, max_buf_size, 0);

                if (nbytes <= 0){   /* error handling for recv: remote connection closed or error */
                    close(pfds_server[i].fd);
                    pfds_server[i].fd = -1;
                }

                else{    /* got good data */
                    memcpy(&message_type, buf, sizeof(message_type)); // get messageType

                    if (ntohl(message_type) == VOTE_RESPONSE) {

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
                    else if (ntohl(message_type) == APPEND_ENTRIES_REQUEST) {

                        appendEntryRequest.UnMarshal(buf);

                        int leader_term = appendEntryRequest.Get_sender_term();
                        int node_term = serverState -> currentTerm;
                        int leader_id = appendEntryRequest.Get_leaderId();

                        if (leader_term > node_term) {
                            nodeInfo -> leader_id = leader_id;
                            nodeInfo->role =  FOLLOWER;
                        }

                    }
                }                       /* End got good data */
            }                     /* End events from established connection */
        }                    /* End got ready-to-read from poll() */
    }                    /* End looping through file descriptors */
}

/* return 0 on failure and 1 on success */
int ServerStub::SendRequestVote(ServerState *serverState, NodeInfo *nodeInfo, int fd) {
    RequestVote requestVote;
    int remain_size = requestVote.Size();
    char buf[remain_size];
    int offset = 0;
    int bytes_written;

    FillRequestVote(serverState, nodeInfo, &requestVote);
    requestVote.Marshal(buf);

    while (remain_size > 0){

        try{
            bytes_written = send(fd, buf+offset, remain_size, 0);
            if (bytes_written < 0){
                throw bytes_written;
            }
        }
        catch(int stat){
            return 0;
        }

        offset += bytes_written;
        remain_size -= bytes_written;
    }

    return 1;
}


void ServerStub::FillRequestVote(ServerState * serverState, NodeInfo * nodeInfo, RequestVote *requestVote) {
    int messageType = VOTE_REQUEST;
    int term = serverState -> currentTerm;
    int candidateId = nodeInfo -> node_id;
    int lastLogIndex = serverState -> smr_log.size() - 1;
    int lastLogTerm = serverState -> smr_log.back().logTerm;

    requestVote -> Set(messageType, term, candidateId, lastLogIndex, lastLogTerm);
}



/* -------------------------------Follower Helper Functions-----------------------------------------*/

/* functionalities include:
  ~ non-blocking receive RequestVoteRPC
*/
void ServerStub:: Handle_Poll_Follower(std::vector<Peer_Info> *PeerServerInfo, ServerState *serverState, ServerTimer *timer, NodeInfo *nodeInfo){
    RequestVote requestVote;
    VoteResponse voteResponse;
    CustomerRequest customer_request;
    char buf[customer_request.Size()];
    int messageType;
    int num_peers = nodeInfo -> num_peers;
    ResponseToCustomer response_to_customer;

    for(int i = 0; i <= num_peers; i++) {   /* looping through file descriptors */
        if (pfds_server[i].revents & POLLIN) {            /* got ready-to-read from poll() */


            if (i==0){ /* events at the listening socket */

                Accept_Connection();
            }

            else{ /* events from established connection */

                int nbytes = recv(pfds_server[i].fd, buf, customer_request.Size(), 0);

                if (nbytes <= 0){  /* connection closed or error */

                    close(pfds_server[i].fd);
                    pfds_server.erase(pfds_server.begin() + i);     /* delete */

                }

                else{             /* got good data */

                    memcpy(&messageType, buf, sizeof(messageType));


                    if (ntohl(messageType) == CUSTOMER_REQUEST) {

                      customer_request.UnMarshal(buf);

                      customer_request.Print();
                      pollfd p;
                      pfds_server.push_back(p);

                      response_to_customer.Set(SENDER_FOLLOWER, nodeInfo->leader_id);

                      SendResponseToCustomer(&response_to_customer, pfds_server[i].fd);


                    }

                    else if(ntohl(messageType) == VOTE_REQUEST) {
                      requestVote.Unmarshal(buf);
                      requestVote.Print();

                      timer->Print_elapsed_time();

                      messageType = VOTE_RESPONSE;
                      voteResponse.Set(messageType, serverState->currentTerm,
                                       Decide_Vote(serverState, nodeInfo, &requestVote),
                                       nodeInfo->node_id);

                      SendVoteResponse(&voteResponse, pfds_server[i].fd);
                    }
                } /* End got good data */
            } /* End events from established connection */

            timer -> Restart();

        } /* End got ready-to-read from poll() */
    } /*  End looping through file descriptors */
}


bool ServerStub::Decide_Vote(ServerState *serverState, NodeInfo *nodeInfo, RequestVote *requestVote) {
    bool result = false;
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
bool ServerStub::Compare_Log(ServerState *serverState, NodeInfo * nodeInfo,RequestVote * requestVote) {

    int candidate_last_log_term = requestVote -> Get_last_log_term();
    int candidate_last_log_index = requestVote -> Get_last_log_index();

    int local_last_log_term = serverState -> smr_log.back().logTerm;
    int local_last_log_index = serverState -> smr_log.size() -1;

    bool greater_last_log_term = candidate_last_log_term > local_last_log_term;
    bool check_last_log_index = candidate_last_log_index >= local_last_log_index;

    bool result = false;

    if (greater_last_log_term){
        result = true;
    }

    else if (candidate_last_log_term == local_last_log_term){
        return check_last_log_index;
    }
    return result;
}

/* return 0 on failure and 1 on success */
int ServerStub::SendVoteResponse(VoteResponse *voteResponse, int fd) {
    int remain_size = voteResponse -> Size();
    char buf[remain_size];
    int offset = 0;
    int bytes_written;

    voteResponse->Marshal(buf);

    while (remain_size > 0){
        try{
            bytes_written = send(fd, buf+offset, remain_size, 0);
            if (bytes_written < 0){
                throw bytes_written;
            }
        }
        catch(int stat){
            return 0;
        }
        offset += bytes_written;
        remain_size -= bytes_written;
    }
    return 1;
}



int ServerStub::SendResponseToCustomer(ResponseToCustomer *response_to_customer_from_follower, int fd) {
  int remain_size = response_to_customer_from_follower -> Size();
  char buf[remain_size];
  int offset = 0;
  int bytes_written;

  response_to_customer_from_follower->Marshal(buf);

  while (remain_size > 0){
    try{
      bytes_written = send(fd, buf+offset, remain_size, 0);
      if (bytes_written < 0){
        throw bytes_written;
      }
    }
    catch(int stat){
      return 0;
    }
    offset += bytes_written;
    remain_size -= bytes_written;
  }
  return 1;
}
/*---------------Log Replication Helper Functions---------------------------------- */
