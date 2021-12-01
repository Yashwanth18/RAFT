#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerFollowerStub.h"
#include "ServerOutStub.h"

/* Massive to-do: check all race condition. Timer, serverState, nodeInfo */

void Raft::
Follower_ListeningThread(ServerSocket *serverSocket, ServerState *serverState,
                         std::vector<std::thread> *thread_vector, ServerTimer *timer){
    while (true) {
        std::unique_ptr<ServerSocket> new_socket;
        new_socket = serverSocket -> Accept();
        std::cout << "\nAccepted Connection from peer server" << '\n';

        std::thread follower_thread(&Raft::FollowerThread, this,
                                    std::move(new_socket), serverState, timer);

        thread_vector -> push_back(std::move(follower_thread));
    }
}

void Raft::FollowerThread(std::unique_ptr<ServerSocket> socket,
                          ServerState *serverState, ServerTimer *timer) {

    int messageType;
    ServerFollowerStub serverFollowerStub;
    serverFollowerStub.Init(std::move(socket));

    messageType = serverFollowerStub.Read_MessageType();

    if (messageType == VOTE_REQUEST) { // main functionality
        timer -> Atomic_Restart();
        serverFollowerStub.Handle_VoteRequest(serverState);
    }

    else if (messageType == APPEND_ENTRY_REQUEST){
        timer -> Atomic_Restart();
        serverFollowerStub.Handle_AppendEntryRequest(serverState);
    }

}

void Raft::CandidateThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                           NodeInfo *nodeInfo, ServerState *serverState) {

    ServerOutStub Out_stub;
    std::string peer_IP;
    int peer_port;
    int messageType = 0;
    bool socket_status;

    peer_IP = (*PeerServerInfo)[peer_index].IP;
    peer_port = (*PeerServerInfo)[peer_index].port;

    bool job_done = false;
    while (!job_done){        // to-do: while true and break when remote socket closed

        socket_status = Out_stub.Init(peer_IP, peer_port);

        if (socket_status) {
            socket_status = Out_stub.Send_MessageType(VOTE_REQUEST);
        }

        if (socket_status) {
            socket_status = Out_stub.Send_RequestVote(serverState, nodeInfo);
        }

        if (socket_status){
            messageType = Out_stub.Read_MessageType();

            if (messageType == RESPONSE_VOTE) {
                lock_print.lock();
                socket_status = Out_stub.Handle_ResponseVote(serverState, &lock_serverState);
                lock_print.unlock();
            }
        }

        if (socket_status){
            job_done = true;
        }

    }
}


void Raft::
LeaderThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
             NodeInfo *nodeInfo, ServerState *serverState) {

    ServerOutStub Out_stub;
    std::string peer_IP;
    int peer_port;
    int socket_status;
    int messageType;
    int heartbeat = 1;

    peer_IP = (*PeerServerInfo)[peer_index].IP;
    peer_port = (*PeerServerInfo)[peer_index].port;

    bool job_done = false;
    while (!job_done){       // to-do: this is really an infinite loop

        socket_status = Out_stub.Init(peer_IP, peer_port);

        if (socket_status){
            socket_status = Out_stub.Send_MessageType(APPEND_ENTRY_REQUEST);
        }

        if (socket_status){
            socket_status = Out_stub.SendAppendEntryRequest(
                                serverState, nodeInfo, peer_index, heartbeat);
        }

        if (socket_status){
            messageType = Out_stub.Read_MessageType();

            if (messageType == RESPONSE_APPEND_ENTRY){
                socket_status = Out_stub.Handle_ResponseAppendEntry(serverState, peer_index);
            }
        }

        if (socket_status){
            job_done = true;
        }
    }
}



