#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerFollowerStub.h"
#include "ServerOutStub.h"


void Election::
FollowerThread(std::unique_ptr<ServerSocket> socket, NodeInfo *nodeInfo, ServerState *serverState) {

    ServerFollowerStub serverFollowerStub;
    serverFollowerStub.Init(std::move(socket));

    int max_data_size = sizeof(AppendEntryRequest) + sizeof(ResponseAppendEntry) +
                        sizeof(VoteRequest) + sizeof(ResponseVote);
    char buf[max_data_size];
    int messageType;

    messageType = serverFollowerStub.Get_MessageType(buf, max_data_size);

    if (messageType == VOTE_REQUEST){ // main functionality
        serverFollowerStub.Handle_VoteRequest(serverState, nodeInfo, buf);
    }

}


void Election::
CandidateThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                NodeInfo *nodeInfo, ServerState *serverState) {

    bool socket_status;
    ServerOutStub Out_stub;
    std::string peer_IP = (*PeerServerInfo)[peer_index].IP;
    int peer_port = (*PeerServerInfo)[peer_index].port;

    socket_status = Out_stub.Init(peer_IP, peer_port);
    std::cout << "Socket_Status: " << socket_status << '\n';

    socket_status = Out_stub.Send_RequestVote(serverState, nodeInfo);
    std::cout << "Socket_Status: " << socket_status << '\n';
}


void Election::
LeaderThread() {

}