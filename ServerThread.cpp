#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerInStub.h"
#include "ServerOutStub.h"
#include "ServerTimer.h"

void Raft::
ListeningThread(ServerSocket *serverSocket, ServerState *serverState,
                std::vector<std::thread> *thread_vector, ServerTimer *timer){
    while (true) {
        std::unique_ptr<ServerSocket> new_socket;
        new_socket = serverSocket -> Accept();
        std::cout << "Accepted Connection from peer server" << '\n';

        std::thread incoming_thread(&Raft::IncomingThread, this,
                                    std::move(new_socket), serverState,
                                    timer);

        thread_vector -> push_back(std::move(incoming_thread));
    }
}




void Raft::
IncomingThread(std::unique_ptr<ServerSocket> socket,
               ServerState *serverState,
               ServerTimer *timer) {

    int messageType;
    int socket_status;
    int _role;
    ServerInStub In_Stub;
    ServerTimer _timer;
    In_Stub.Init(std::move(socket));

    timer -> Atomic_Restart();

    while(!timer -> Check_Election_timeout()){      // for follower thread

        messageType = In_Stub.Read_MessageType();

        if (messageType == 0){
            break;
        }

        if (messageType == VOTE_REQUEST) { // main functionality
            timer -> Atomic_Restart();
            socket_status = In_Stub.Handle_VoteRequest(serverState);
        }

        else if (messageType == APPEND_ENTRY_REQUEST){
            timer -> Atomic_Restart();
            socket_status = In_Stub.Handle_AppendEntryRequest(serverState);
        }

        serverState -> lck.lock();       // lock
        _role = serverState -> role;
        serverState -> lck.unlock();     // unlock

        if (_role == CANDIDATE){        // for candidate thread receiving AppendEntryRequest from leader
            break;
        }

        if(!socket_status){
            break;
        }
    }
    // std::cout << "Exiting IncomingThread that handled peer servers \n" << '\n';

}

void Raft::
CandidateThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                NodeInfo *nodeInfo, ServerState *serverState) {

    ServerTimer _timer;
    int still_trying;

    _timer.Start();

    while(!_timer.Check_Election_timeout()){

        serverState -> lck.lock();         // lock
        if (serverState -> role == LEADER){
            break;
        }
        serverState -> lck.unlock();       // unlock

        still_trying = Candidate_Quest(peer_index, PeerServerInfo, nodeInfo,
                                       serverState);
        if (still_trying != 1){
            break;
        }
    }
    // std::cout << "Exiting Candidate Thread" << '\n';

}

int Raft::Candidate_Quest(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                          NodeInfo *nodeInfo, ServerState *serverState){
    bool socket_status;
    ServerOutStub outStub;
    std::string peer_IP;
    int peer_port;
    int messageType;
    int still_trying = 1;

    peer_IP = (*PeerServerInfo)[peer_index].IP;
    peer_port = (*PeerServerInfo)[peer_index].port;

    socket_status = outStub.Init(peer_IP, peer_port);
    if (socket_status) {
        socket_status = outStub.Send_MessageType(VOTE_REQUEST);
    }

    if (socket_status) {
        socket_status = outStub.Send_RequestVote(serverState, nodeInfo);
    }

    if (socket_status){
        messageType = outStub.Read_MessageType();

        if (messageType == RESPONSE_VOTE) {
            socket_status = outStub.Handle_ResponseVote(serverState);
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
             NodeInfo *nodeInfo, ServerState *serverState, Bridge *bridge) {

    ServerOutStub outStub;
    std::string peer_IP;
    int peer_port;
    int socket_status;
    int messageType;
    bool job_done;
    int _role;
    int matchIndex;
    int lastLogIndex;

    peer_IP = (*PeerServerInfo)[peer_index].IP;
    peer_port = (*PeerServerInfo)[peer_index].port;

    while (true){
        // to-do: wait for client's request
        job_done = false;
        int heartbeat = 1;

        serverState -> lck.lock();  // lock
        matchIndex = serverState -> matchIndex[peer_index];
        lastLogIndex = serverState -> smr_log.size() - 1;
        serverState -> lck.unlock();    // unlock

        while(matchIndex < lastLogIndex){

        }


        while(!job_done) {
            socket_status = outStub.Init(peer_IP, peer_port);

            if (socket_status) {
                socket_status = outStub.Send_MessageType(APPEND_ENTRY_REQUEST);
            }

            if (socket_status) {
                socket_status = outStub.SendAppendEntryRequest(
                        serverState, nodeInfo, peer_index, heartbeat);
            }

            if (socket_status) {
                messageType = outStub.Read_MessageType();

                if (messageType == RESPONSE_APPEND_ENTRY) {
                    socket_status = outStub.Handle_ResponseAppendEntry(
                                serverState, peer_index, nodeInfo);

                    if (socket_status) {
                        job_done = true;
                    }
                }
            }

            serverState -> lck.lock(); // lock
            _role  = serverState -> role;
            serverState -> lck.unlock(); // unlock

            if (_role == FOLLOWER){     // if we detect that we are stale
                break;
            }

        } // End: while (!job_done)

        if (_role == FOLLOWER){         // if we detect that we are stale
            break;
        }

    }  // End: while (true)
}

void Raft::Apply_Committed_Op(ServerState *serverState, std::map<int, int> MapCustomerRecord,
                              std::mutex *lk_Map){
    int commitIndex;
    LogEntry logEntry;

    serverState -> lck.lock();       // lock
    commitIndex = serverState -> commitIndex;
    logEntry = serverState -> smr_log.at(commitIndex);
    serverState -> lck.unlock();     // unlock

    MapOp mapOp_commited = {logEntry.opcode, logEntry.arg1, logEntry.arg2};

    if (mapOp_commited.opcode == 1){    // only support update value operation for now
        lk_Map -> lock();   // lock
        MapCustomerRecord [mapOp_commited.arg1] = mapOp_commited.arg2;
        lk_Map -> unlock();  // unlock
    }
}


