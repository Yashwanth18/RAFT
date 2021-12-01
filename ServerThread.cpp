#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerFollowerStub.h"
#include "ServerOutStub.h"

void Election::
Follower_ListeningThread(ServerSocket *serverSocket, NodeInfo *nodeInfo,
                         ServerState *serverState,
                         std::vector<std::thread> *thread_vector){
    while (true) {
        std::unique_ptr<ServerSocket> new_socket;
        new_socket = serverSocket -> Accept();
        std::cout << "\nAccepted Connection from peer server" << '\n';

        std::thread follower_thread(&Election::FollowerThread, this,
                                    std::move(new_socket), nodeInfo, serverState);

        thread_vector -> push_back(std::move(follower_thread));
    }
}

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
    else if (messageType == APPEND_ENTRY_REQUEST){

        serverFollowerStub.Handle_AppendEntryRequest(serverState, nodeInfo);

    }

}

void Election::
LeaderThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
             NodeInfo *nodeInfo, ServerState *serverState, bool *sent) {

    ServerOutStub Out_stub;
    std::unique_lock<std::mutex> ul(lock_state, std::defer_lock);
    std::string peer_IP;
    int peer_port;
    int messageType;
    int heartbeat = 1;

    ul.lock();  // debugging purposes only!

    if (!*sent){

        peer_IP = (*PeerServerInfo)[peer_index].IP;
        peer_port = (*PeerServerInfo)[peer_index].port;
        Out_stub.Init(peer_IP, peer_port);

        int send_status = Out_stub.Send_MessageType(APPEND_ENTRY_REQUEST);

        if (send_status){
            *sent = true;
            Out_stub.SendAppendEntryRequest(serverState, nodeInfo, peer_index, heartbeat);
        }

        messageType = Out_stub.Read_MessageType();
        std::cout << "messageType: " << messageType << '\n';

        if (messageType == RESPONSE_APPEND_ENTRY){
            Out_stub.Handle_ResponseAppendEntry(serverState, peer_index);
        }

    }
    ul.unlock();    // debugging purposes only!
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