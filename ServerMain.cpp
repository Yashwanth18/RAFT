#include "ServerMain.h"

int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo nodeInfo;
    Raft raft; /* All server threads are here: Follower, Candidate, Leader */
    ServerSocket serverSocket;
    ServerState serverState;
    ServerSocket clientSocket;
    Interface interface;
    std::vector<Peer_Info> PeerServerInfo;
    MapClientRecord mapRecord;
    std::vector <std::thread> thread_vector;

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo))           { return 0; }
    if (!Init_NodeInfo(&nodeInfo, argc, argv))                      { return 0; }
    Init_ServerState(&serverState, nodeInfo.num_peers, argc, argv);

    /* init a listening port to listen to peer servers */
    if (!serverSocket.Init(nodeInfo.server_port)) {
        std::cout << "Socket initialization failed" << '\n';
        return 0;
    }

    /* init a listening port to listen to customer */
    if (!clientSocket.Init(nodeInfo.client_port)) {
        std::cout << "Socket initialization failed" << '\n';
        return 0;
    }

    std::thread listen_clientThread(&Interface::Listening_Client, &interface,
                                    &clientSocket, &serverState,
                                    &thread_vector, &mapRecord);
    thread_vector.push_back(std::move(listen_clientThread));


    std::thread listenThread(&Raft::ListeningThread, &raft, &serverSocket, &serverState,
                             &thread_vector,  &timer, &mapRecord);
    thread_vector.push_back(std::move(listenThread));


    while(true){
        timer.Atomic_Restart();

        if (serverState.role == FOLLOWER) {
            while(!timer.Check_Election_timeout()){}    /* do nothing */
            SetRole_Atomic(&serverState, CANDIDATE);    /* if election timeout */
        }

        else if (serverState.role == CANDIDATE) {
            Candidate_Role(&serverState, &nodeInfo, &PeerServerInfo,
                           &thread_vector, &raft);
        }

        else if (serverState.role == LEADER) {
            // to-do: upon election: send a heartbeat message. engineer the initialization

            for (int i = 0; i < nodeInfo.num_peers; i++){
                std::thread leader_thread(&Raft::LeaderThread, &raft, i, &PeerServerInfo,
                                          &nodeInfo, &serverState, &mapRecord);
                thread_vector.push_back(std::move(leader_thread));
            }

            /* do nothing :just so that we do not spin infinite number of threads */
            while (true) {}
        }

        else {
            std::cout << "Undefined Server Role Initialization" << '\n';
        }
    }

}  // End: Main

/* -------------------------------End: Main Function------------------------------------ */

/* ------------------Candidate Helper Functions  ------------------*/
void NewElection_Atomic(ServerState *serverState, NodeInfo *nodeInfo){
    serverState -> lck.lock();
    serverState -> currentTerm ++;
    serverState -> num_votes = 1;
    serverState -> votedFor = nodeInfo -> node_id; // vote for itself
    serverState -> lck.unlock();
}



void Candidate_Role(ServerState *serverState, NodeInfo *nodeInfo,
                    std::vector<Peer_Info> *PeerServerInfo,
                    std::vector <std::thread> *thread_vector,
                    Raft *raft){

    int half_peers = nodeInfo -> num_peers / 2;
    int num_votes;
    ServerTimer _timer;

    NewElection_Atomic(serverState, nodeInfo);

    for (int i = 0; i < nodeInfo -> num_peers; i++){
        std::thread candidate_thread(&Raft::CandidateThread, raft, i, PeerServerInfo,
                                     nodeInfo, serverState);
        thread_vector -> push_back(std::move(candidate_thread));
    }

    _timer.Start();

    while(!_timer.Check_Election_timeout()){

        serverState -> lck.lock();   // lock
        num_votes = serverState -> num_votes;
        serverState -> lck.unlock();   // unlock

        if (num_votes > half_peers){

            serverState -> lck.lock();   // lock
            serverState -> role = LEADER;
            serverState -> leader_id = nodeInfo -> node_id;
            std::cout << "Becoming a leader now!" << '\n';
            serverState -> lck.unlock();   // unlock
            break;
        }
    }

    serverState -> lck.lock();   // lock
    if (serverState -> role != LEADER) {
        std::cout << "Candidate timeout: resigning to be a follower" << '\n';
        serverState -> role = FOLLOWER;
    }
    serverState -> lck.unlock();   // unlock

}