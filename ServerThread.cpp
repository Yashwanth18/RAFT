#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerInStub.h"
#include "ServerOutStub.h"
#include "ServerTimer.h"

void Raft::
ListeningThread(ServerSocket *serverSocket, ServerState *serverState,
                std::vector<std::thread> *thread_vector, ServerTimer *timer,
                MapClientRecord *mapRecord){
    while (true) {
        std::unique_ptr<ServerSocket> new_socket;
        new_socket = serverSocket -> Accept();
        // std::cout << "Accepted Connection from peer server" << '\n';

        std::thread incoming_thread(&Raft::IncomingThread, this,
                                    std::move(new_socket), serverState,
                                    timer, mapRecord);

        thread_vector -> push_back(std::move(incoming_thread));
    }
}




void Raft::
IncomingThread(std::unique_ptr<ServerSocket> socket,
               ServerState *serverState,
               ServerTimer *timer, MapClientRecord *mapRecord) {

    int messageType;
    int socket_status;

    ServerInStub In_Stub;
    ServerTimer _timer;
    In_Stub.Init(std::move(socket));

    timer -> Atomic_Restart();

    while(!timer -> Check_Election_timeout()){      // for follower thread

        Apply_Committed_Op(serverState, mapRecord);

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


        /* if candidate thread receiving AppendEntryRequest from leader */
        if (serverState -> GetRole() == CANDIDATE){
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

        if (serverState -> GetRole() == LEADER){
            break;
        }


        still_trying = Candidate_Quest(peer_index, PeerServerInfo, nodeInfo,
                                       serverState);
        if (still_trying != 1){
            break;
        }
    }
    std::cout << "Exiting Candidate Thread" << '\n';

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
             NodeInfo *nodeInfo, ServerState *serverState,
             MapClientRecord *mapRecord, ServerTimer *timer) {


    ServerOutStub outStub;
    std::string peer_IP;
    int peer_port;
    int socket_status;
    int messageType;
    bool heartbeat;
    bool upToDate;

    peer_IP = (*PeerServerInfo)[peer_index].IP;
    peer_port = (*PeerServerInfo)[peer_index].port;


    while (true){
        /* wait for client's request or send heartbeat */
        socket_status = outStub.Init(peer_IP, peer_port);

        upToDate = Check_UpToDate(serverState, peer_index);
        heartbeat = upToDate;

        if (socket_status) {
            socket_status = outStub.Send_MessageType(APPEND_ENTRY_REQUEST);
        }

        if (socket_status) {
            socket_status = outStub.SendAppendEntryRequest(
                    serverState, nodeInfo, peer_index, heartbeat);
        }

        if (socket_status) {
            messageType = outStub.Read_MessageType();
        }

        if (messageType == RESPONSE_APPEND_ENTRY) {
            outStub.Handle_ResponseAppendEntry(
                    serverState, peer_index, nodeInfo);
        }

        Apply_Committed_Op(serverState, mapRecord);



        /* if leader detects that it is stale */
        if (serverState -> GetRole() == FOLLOWER){
            break;
        }
    }  // End: while (true)
}

bool Raft::Check_UpToDate(ServerState *serverState, int peer_index) {
    int matchIndex;
    int lastLogIndex;

    serverState -> lck.lock();  // lock
    matchIndex = serverState -> matchIndex[peer_index];
    lastLogIndex = serverState -> smr_log.size() - 1;
    serverState -> lck.unlock();    // unlock

//    std::cout << "matchIndex: " << matchIndex << '\n';
//    std::cout << "lastLogIndex: " << lastLogIndex << '\n';

    if (matchIndex < lastLogIndex){
        return false;
    }
    else{
        return true;
    }
}

void Raft::Apply_Committed_Op(ServerState *serverState, MapClientRecord *mapRecord){
    int commitIndex;
    int lastApplied;
    int lastLogIndex;

    serverState -> lck.lock();      // lock
    commitIndex = serverState -> commitIndex;
    lastApplied = serverState -> lastApplied;
    lastLogIndex = serverState -> smr_log.size() - 1;
    serverState -> lck.unlock();    // unlock

    if (lastApplied < commitIndex){

        if (lastLogIndex < commitIndex){
            commitIndex = lastLogIndex;
        }
        serverState -> lck.lock();       // lock
        for (int i = commitIndex; i <= lastLogIndex; i++){
            serverState -> lastApplied = serverState -> commitIndex;
            LogEntry logEntry = serverState -> smr_log.at(commitIndex);
            MapOp mapOp_commited = {logEntry.opcode, logEntry.arg1, logEntry.arg2};

            mapRecord -> lck.lock();	   // lock
            mapRecord -> Record_Dict [mapOp_commited.arg1] = mapOp_commited.arg2;
            mapRecord -> lck.unlock();	 // unlock
        }
        serverState -> lck.unlock();     // unlock
    }
}


