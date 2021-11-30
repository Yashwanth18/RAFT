#include"ServerStub.h"

/* Return 1 on success and 0 on failure */
void ServerStub::Init(NodeInfo * nodeInfo){
    /* Listen for both the clients and the peer servers through one socket */
    AddSocketToPoll(ListenSocket.Init(nodeInfo -> server_port), &pfds_server);

    for (int i = 0; i < nodeInfo -> num_peers; i++){
        AddSocketToPoll(-1, &pfds_server);
    }
}

void ServerStub:: Set_Pfd(int new_fd, int peer_index){
    int num_sockets = pfds_server.size();
    pollfd new_pfd;
    new_pfd.fd = new_fd;
    new_pfd.events = POLLIN;

    if (peer_index + 1 < num_sockets){
        perror("Set_Pfd");
    }
    pfds_server[peer_index + 1] = new_pfd;
}

int ServerStub::Poll(int poll_timeout){
    int poll_count = poll(pfds_server.data(), pfds_server.size(), poll_timeout);
    if ( poll_count < 0 )   perror("poll");
    return poll_count;
}

/* ---------------------------------Follower Helper function: ---------------------------------- */
void ServerStub::
Handle_Poll_Follower(ServerTimer *Timer, ServerState *serverState, NodeInfo *nodeInfo,
                     int *Socket, bool *Is_Init, bool *Socket_Status){
    serverFollowerStub.Stub_Handle_Poll_Follower(Timer, &pfds_server, serverState, nodeInfo,
                                                 Socket, Is_Init, Socket_Status);
}

/* ---------------------------------Candidate helper functions -----------------------------------*/
void ServerStub::Handle_Poll_Candidate(ServerState * serverState, std::map<int,int> *PeerIdIndexMap,
                                       bool *VoteRequest_Completed, NodeInfo *nodeInfo,
                                       int *Socket, bool *Is_Init, bool *Socket_Status){

    serverCandidateStub.Stub_Handle_Poll_Candidate(&pfds_server, serverState, PeerIdIndexMap,
                                                   VoteRequest_Completed, nodeInfo,
                                                   Socket, Is_Init, Socket_Status);
}

int ServerStub::SendVoteRequest(ServerState *serverState, NodeInfo *nodeInfo, int fd){
    return serverCandidateStub.Stub_SendVoteRequest(serverState, nodeInfo, fd);
}

/* ---------------------------------Leader helper function---------------------------------- */
/* functionalities include: Receive acknowledgement from Follower */
void ServerStub::
Handle_Poll_Leader(ServerState *serverState, NodeInfo *nodeInfo, std::map<int,int> *PeerIdIndexMap,
                   int * LogRep_RequestID, int *Socket, bool *Is_Init, bool *Socket_Status){
    serverLeaderStub.Stub_Handle_Poll_Leader(&pfds_server, nodeInfo, serverState, PeerIdIndexMap,
                                             LogRep_RequestID, Socket, Is_Init, Socket_Status);
}

int ServerStub::
SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo, int fd, int peer_index, int LogRep_RequestID) {
  return serverLeaderStub.Stub_SendAppendEntryRequest(serverState, nodeInfo, fd, peer_index, LogRep_RequestID);
}



