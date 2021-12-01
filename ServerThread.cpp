#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerFollowerStub.h"
#include "ServerOutStub.h"

/* Massive to-do: check all race condition. Timer, serverState, nodeInfo */

void Raft::
Follower_ListeningThread(ServerSocket *serverSocket, NodeInfo *nodeInfo,
                         ServerState *serverState,
                         std::vector<std::thread> *thread_vector, ServerTimer *timer){
    while (true) {
        std::unique_ptr<ServerSocket> new_socket;
        new_socket = serverSocket -> Accept();
        std::cout << "\nAccepted Connection from peer server" << '\n';

        std::thread follower_thread(&Raft::FollowerThread, this,
                                    std::move(new_socket), nodeInfo,
                                    serverState, timer);

        thread_vector -> push_back(std::move(follower_thread));
    }
}

void Raft::
FollowerThread(std::unique_ptr<ServerSocket> socket, NodeInfo *nodeInfo,
               ServerState *serverState, ServerTimer *timer) {

    int messageType;
    ServerFollowerStub serverFollowerStub;
    std::unique_lock<std::mutex> ul(lock_timer, std::defer_lock);

    serverFollowerStub.Init(std::move(socket));

    //while(true){    // break if connection closed by the other sides
//        ul.lock();
//        timer->Restart(); // debug only!
//        ul.unlock();

        messageType = serverFollowerStub.Read_MessageType();
        std::cout << "messageType: " << messageType << '\n';

        if (messageType == VOTE_REQUEST) { // main functionality
            /* handle vote request */
            serverFollowerStub.Handle_VoteRequest(serverState);
        }

        else if (messageType == APPEND_ENTRY_REQUEST){
            timer->Restart();
            serverFollowerStub.Handle_AppendEntryRequest(serverState, nodeInfo);

        }
    //}
}

void Raft::
LeaderThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
             NodeInfo *nodeInfo, ServerState *serverState, bool *sent) {

    ServerOutStub Out_stub;
    std::unique_lock<std::mutex> ul(lock_serverState, std::defer_lock);
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



void Raft::
CandidateThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                NodeInfo *nodeInfo, ServerState *serverState, bool *sent) {

    ServerOutStub Out_stub;
    std::unique_lock<std::mutex> ul(lock_serverState, std::defer_lock);
    std::string peer_IP;
    int peer_port;
    int messageType;
    bool socket_status;

    ul.lock();  // debugging purposes only!

    if (!*sent){        // to-do: while true and break when remote socket closed

        peer_IP = (*PeerServerInfo)[peer_index].IP;
        peer_port = (*PeerServerInfo)[peer_index].port;
        socket_status = Out_stub.Init(peer_IP, peer_port);

        socket_status = Out_stub.Send_MessageType(VOTE_REQUEST);

        if (socket_status){
            *sent = true;
            Out_stub.Send_RequestVote(serverState, nodeInfo);
        }

        messageType = Out_stub.Read_MessageType();
        std::cout << "messageType: " << messageType << '\n';

        if (messageType == RESPONSE_VOTE){
            Out_stub.Handle_ResponseVote(nodeInfo, serverState);
        }
    }
    ul.unlock();    // debugging purposes only!
}