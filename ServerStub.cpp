#include"ServerStub.h"

/*
Initialize a non-blocking socket to listen for connection from peer servers and fill peer server info
Return 1 on success and 0 on failure
*/
void ServerStub:: Init(NodeInfo * nodeInfo){

  port = nodeInfo -> server_port;
  num_peers = nodeInfo -> num_peers;

  /* Listen for both the clients and the peer servers through one socket */
  Add_Socket_To_Poll(ListenSocket.Init(port));
}

void ServerStub:: Add_Socket_To_Poll(int new_fd){
    pollfd new_pfd;
    new_pfd.fd = new_fd;
    new_pfd.events = POLLIN;
    pfds_server.push_back(new_pfd);
}

/* return the new file descriptor
 * 0 on failure and 1 on success
 * */

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

    return 1;   /* to-do: fix this with socket_status */
}

void ServerStub::FillRequestVote(ServerState * serverState, NodeInfo * nodeInfo, RequestVote *requestVote) {
    int messageType = VOTE_REQUEST;
    int term = serverState -> currentTerm;
    int candidateId = nodeInfo -> node_id;
    int lastLogIndex = serverState -> smr_log.size() - 1;
    int lastLogTerm = serverState -> smr_log.back().logTerm;

    requestVote -> Set(messageType, term, candidateId, lastLogIndex, lastLogTerm);
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

/* return the new file descriptor
 * 0 on failure and 1 on success
 * */
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

  //the listening socket is pfds_server[0].fd
  new_fd = accept(pfds_server[0].fd, (struct sockaddr *) &addr, &addr_size);
  if (new_fd < 0) perror ("accept");

  Add_Socket_To_Poll(new_fd);
}



int ServerStub:: Poll(int poll_timeout){
    int poll_count = poll(pfds_server.data(), pfds_server.size(), poll_timeout);
    if( poll_count < 0 )   perror("poll");
    return poll_count;
}



/* functionalities include:
  ~ non-blocking receive VoteResponse
*/
void ServerStub:: Handle_Poll_Peer(ServerState * serverState, std::map<int,int> *PeerIdIndexMap,
                                   bool *request_completed,
                                   NodeInfo *nodeInfo){
    VoteResponse voteResponse;
    AppendEntryRequest appendEntryRequest;

    char buf[voteResponse.Size() + appendEntryRequest.Size()];
    int num_alive_sockets = pfds_server.size();
    int message_type; // message descriptor to determine if the message is append entries or vote response
    int peer_index;

    for(int i = 0; i < num_alive_sockets; i++) {   /* looping through file descriptors */
        if (pfds_server[i].revents & POLLIN) {     /* got ready-to-read from poll() */

            if (i==0){                             /* events at the listening socket */
                Accept_Connection();
            }

            else{                                   /* events from established connection */
                int nbytes = recv(pfds_server[i].fd, buf, sizeof(voteResponse), 0);

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






