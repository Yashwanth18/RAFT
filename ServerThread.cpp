#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerFollowerStub.h"
#include "ServerOutStub.h"


void Election::
FollowerThread(std::unique_ptr<ServerSocket> socket, NodeInfo *nodeInfo, ServerState *serverState) {
    int messageType;
    ServerFollowerStub serverFollowerStub;
    serverFollowerStub.Init(std::move(socket));

    messageType = serverFollowerStub.Read_MessageType();
    std::cout << "messageType: " << messageType << '\n';

    if (messageType == VOTE_REQUEST) { // main functionality
        /* handle vote request */

        serverFollowerStub.Send_MessageType(RESPONSE_VOTE);
    }

}


void Election::
CandidateThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                NodeInfo *nodeInfo, ServerState *serverState, bool *sent) {

    ServerOutStub Out_stub;
    std::unique_lock<std::mutex> ul(lock_state, std::defer_lock);
    std::string peer_IP;
    int peer_port;
    int messageType;

    ul.lock();  // debugging purposes only!

    if (!*sent){

        peer_IP = (*PeerServerInfo)[peer_index].IP;
        peer_port = (*PeerServerInfo)[peer_index].port;
        Out_stub.Init(peer_IP, peer_port);

        int send_status = Out_stub.Send_MessageType(VOTE_REQUEST);

        if (send_status){
            *sent = true;
            messageType = Out_stub.Read_MessageType();
            std::cout << "messageType: " << messageType << '\n';

        }
    }
    ul.unlock();    // debugging purposes only!
}



void Election::
LeaderThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
             NodeInfo *nodeInfo, ServerState *serverState, bool *sent) {

    ServerOutStub Out_stub;
    std::unique_lock<std::mutex> ul(lock_state, std::defer_lock);
    std::string peer_IP;
    int peer_port;
    int messageType;

    ul.lock();  // debugging purposes only!

    if (!*sent){

        peer_IP = (*PeerServerInfo)[peer_index].IP;
        peer_port = (*PeerServerInfo)[peer_index].port;
        Out_stub.Init(peer_IP, peer_port);

        int send_status = Out_stub.Send_MessageType(VOTE_REQUEST);

        if (send_status){
            *sent = true;
            messageType = Out_stub.Read_MessageType();
            std::cout << "messageType: " << messageType << '\n';

        }
    }
    ul.unlock();    // debugging purposes only!
}
