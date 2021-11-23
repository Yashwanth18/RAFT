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

int ServerStub:: Poll(int poll_timeout){
    int poll_count = poll(pfds_server.data(), pfds_server.size(), poll_timeout);
    if( poll_count < 0 )   perror("poll");
    return poll_count;
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

    // the listening socket is pfds_server[0].fd
    new_fd = accept(pfds_server[0].fd, (struct sockaddr *) &addr, &addr_size);
    if (new_fd < 0) perror ("accept");

    Add_Socket_To_Poll(new_fd);
}

int ServerStub::
SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo,
                       int fd, int peer_index, int * RequestID) {

    AppendEntryRequest appendEntryRequest;
    int remain_size = appendEntryRequest.Size();
    char buf[remain_size];
    int offset = 0;
    int bytes_written;

    FillAppendEntryRequest(serverState, nodeInfo, &appendEntryRequest, peer_index, RequestID);
    appendEntryRequest.Marshal(buf);

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


void ServerStub::
FillAppendEntryRequest(ServerState * serverState, NodeInfo * nodeInfo,
                       AppendEntryRequest *appendEntryRequest,  int peer_index, int * RequestID) {

    int _messageType = APPEND_ENTRY_REQUEST;
    int _sender_term = serverState -> currentTerm;
    int _leaderId = nodeInfo -> node_id;
    int _leaderCommit = serverState -> commitIndex;
    int nextIndexPeer = serverState->nextIndex[peer_index];

    LogEntry logEntry = serverState -> smr_log.at( nextIndexPeer );

    std::cout << "nextIndex in fill request: " << serverState -> nextIndex[peer_index] << '\n';
    int _prevLogIndex = serverState -> nextIndex[peer_index] - 1;
    int _prevLogTerm = -1;


    if (_prevLogIndex >= 0){
        _prevLogTerm = serverState -> smr_log.at(_prevLogIndex).logTerm;
    }

    appendEntryRequest -> Set(_messageType, _sender_term, _leaderId,
                              _prevLogTerm, _prevLogIndex,
                              &logEntry, _leaderCommit, *RequestID);
}

/* functionalities include:
  ~ non-blocking receive VoteResponse
*/
void ServerStub::
Handle_Poll_Peer(ServerState *serverState, std::map<int,int> *PeerIdIndexMap, int * RequestID){

    AppendEntryResponse appendEntryResponse;

    char buf[appendEntryResponse.Size()];
    int num_alive_sockets = pfds_server.size();
    int peer_index;

    for(int i = 0; i < num_alive_sockets; i++) {   /* looping through file descriptors */
        if (pfds_server[i].revents & POLLIN) {     /* got ready-to-read from poll() */

            if (i==0){                             /* events at the listening socket */
                Accept_Connection();
            }

            else{                                   /* events from established connection */
                int nbytes = recv(pfds_server[i].fd, buf, sizeof(appendEntryResponse), 0);

                if (nbytes <= 0){   /* error handling for recv: remote connection closed or error */
                    close(pfds_server[i].fd);
                    pfds_server[i].fd = -1;
                }

                else{    /* got good data */
                    appendEntryResponse.UnMarshal(buf);
                    appendEntryResponse.Print();

                    peer_index = (*PeerIdIndexMap)[appendEntryResponse.Get_nodeID()];

                    if (appendEntryResponse.Get_success()) {
                        serverState -> nextIndex[peer_index] ++;
                    }
                    else {  // rejected: the follower node lags behind /* to-do */
                        if (appendEntryResponse.Get_ResponseID() > *RequestID){
                            serverState -> nextIndex[peer_index] --;
                            (*RequestID)++;
                        }
                    }

                } /* End got good data */
            } /* End events from established connection */
        } /* End got ready-to-read from poll() */
    } /* End looping through file descriptors */
}






