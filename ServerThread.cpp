#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerFollowerStub.h"
#include "ServerOutStub.h"
#include "ServerTimer.h"

/* Massive to-do: check all race condition. Timer, serverState, nodeInfo */

void Raft::
ListeningThread(ServerSocket *serverSocket, ServerState *serverState,
                         std::vector<std::thread> *thread_vector,
                         std::mutex *lk_serverState, ServerTimer *timer){
    while (true) {
        std::unique_ptr<ServerSocket> new_socket;
        new_socket = serverSocket -> Accept();
        std::cout << "Accepted Connection from peer server" << '\n';

        std::thread follower_thread(&Raft::FollowerThread, this,
                                    std::move(new_socket), serverState,
                                    lk_serverState, timer);

        thread_vector -> push_back(std::move(follower_thread));
    }
}

void Raft::
FollowerThread(std::unique_ptr<ServerSocket> socket,
               ServerState *serverState, std::mutex *lk_serverState,
               ServerTimer *timer) {

    int messageType;
    int socket_status;
    ServerFollowerStub serverFollowerStub;
    ServerTimer _timer;
    serverFollowerStub.Init(std::move(socket));

    timer -> Atomic_Restart();

    while(!timer -> Check_Election_timeout()){
        timer -> Atomic_Restart(); // debugging only! for candidate becoming follower

        messageType = serverFollowerStub.Read_MessageType();

        if (messageType == 0){
            break;
        }


        if (messageType == VOTE_REQUEST) { // main functionality
            timer -> Atomic_Restart();
            socket_status = serverFollowerStub.Handle_VoteRequest(
                                            serverState, lk_serverState);
        }

        else if (messageType == APPEND_ENTRY_REQUEST){
            timer -> Atomic_Restart();
            // to-do: handle for when role = candidate too
            socket_status = serverFollowerStub.Handle_AppendEntryRequest(
                                                serverState, lk_serverState);
        }

        if(!socket_status){
            break;
        }
    }
    std::cout << "Exiting Follower Thread\n" << '\n';

}

void Raft::
CandidateThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                NodeInfo *nodeInfo, ServerState *serverState,
                std::mutex *lk_serverState) {

    ServerTimer _timer;
    int still_trying;

    _timer.Start();

    while(!_timer.Check_Election_timeout()){

        lk_serverState->lock();         // lock
        if (serverState -> role == LEADER){
            break;
        }
        lk_serverState->unlock();       // unlock

        still_trying = Candidate_Quest(peer_index, PeerServerInfo, nodeInfo,
                                   serverState, lk_serverState);
        if (still_trying != 1){
            break;
        }
    }
    std::cout << "Exiting Candidate Thread" << '\n';

}

int Raft::Candidate_Quest(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                          NodeInfo *nodeInfo, ServerState *serverState,
                          std::mutex *lk_serverState){
    bool socket_status;
    ServerOutStub Out_stub;
    std::string peer_IP;
    int peer_port;
    int messageType;
    int still_trying = 1;

    peer_IP = (*PeerServerInfo)[peer_index].IP;
    peer_port = (*PeerServerInfo)[peer_index].port;

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
            socket_status = Out_stub.Handle_ResponseVote(serverState, lk_serverState);
            if (socket_status){
                still_trying = 0;
            }
        }
    }

    if (!socket_status){
        return -1;
    }
    return still_trying;
}


void Raft::
LeaderThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
             NodeInfo *nodeInfo, ServerState *serverState) {

    ServerOutStub Out_stub;
    std::string peer_IP;
    int peer_port;
    int socket_status;
    int messageType;
    bool job_done;

    peer_IP = (*PeerServerInfo)[peer_index].IP;
    peer_port = (*PeerServerInfo)[peer_index].port;

    while (true){
        // to-do: wait for client's request
        job_done = false;
        int heartbeat = 1;

        while(!job_done) {
            socket_status = Out_stub.Init(peer_IP, peer_port);

            if (socket_status) {
                socket_status = Out_stub.Send_MessageType(APPEND_ENTRY_REQUEST);
            }

            if (socket_status) {
                socket_status = Out_stub.SendAppendEntryRequest(
                        serverState, nodeInfo, peer_index, heartbeat);
            }

            if (socket_status) {
                messageType = Out_stub.Read_MessageType();

                if (messageType == RESPONSE_APPEND_ENTRY) {
                    socket_status = Out_stub.Handle_ResponseAppendEntry(serverState, peer_index);

                    if (socket_status) {
                        job_done = true;
                    }
                }
            }
        }
    }
}



