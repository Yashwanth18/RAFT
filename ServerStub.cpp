#include"ServerStub.h"

/* Return 1 on success and 0 on failure */
void ServerStub::Init(NodeInfo * nodeInfo){
    /* Listen for both the clients and the peer servers through one socket */
    Socket_Add_Socket_To_Poll(ListenSocket.Init(nodeInfo -> server_port), &pfds_server);
}

void ServerStub::Add_Socket_To_Poll(int new_fd) {
    Socket_Add_Socket_To_Poll(new_fd, &pfds_server);
}

int ServerStub::Poll(int poll_timeout){
    int poll_count = poll(pfds_server.data(), pfds_server.size(), poll_timeout);
    if ( poll_count < 0 )   perror("poll");
    return poll_count;
}

/* ---------------------------------Follower Helper function: ---------------------------------- */
void ServerStub::Handle_Poll_Follower(ServerState *serverState, NodeInfo *nodeInfo){
    serverFollowerStub.Stub_Handle_Poll_Follower(&pfds_server, serverState, nodeInfo);
}

/* ---------------------------------Candidate helper functions -----------------------------------*/
void ServerStub::Handle_Poll_Candidate(ServerState * serverState, std::map<int,int> *PeerIdIndexMap,
                                       bool *request_completed, NodeInfo *nodeInfo){
    serverCandidateStub.Stub_Handle_Poll_Candidate(&pfds_server, serverState, PeerIdIndexMap,
                                                   request_completed, nodeInfo);
}

int ServerStub::SendRequestVote(ServerState *serverState, NodeInfo *nodeInfo, int fd){
    return serverCandidateStub.Stub_SendRequestVote(serverState, nodeInfo, fd);
}

/* ---------------------------------Leader helper function---------------------------------- */
/* functionalities include: Receive acknowledgement from Follower */
void ServerStub::
Handle_Poll_Leader(ServerState *serverState, std::map<int,int> *PeerIdIndexMap, int * RequestID){
    serverLeaderStub.Stub_Handle_Poll_Leader(&pfds_server, serverState, PeerIdIndexMap, RequestID);
}

int ServerStub::
SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo, int fd, int peer_index, int RequestID) {
  return serverLeaderStub.Stub_SendAppendEntryRequest(serverState, nodeInfo, fd, peer_index, RequestID);
}



