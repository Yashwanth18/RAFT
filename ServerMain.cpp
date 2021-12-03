#include "ServerMain.h"

int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo nodeInfo;
    ServerState serverState;
    ServerSocket serverSocket;
    ServerSocket clientSocket;
    std::vector<Peer_Info> PeerServerInfo;
    std::vector <std::thread> thread_vector;
    std::mutex lk_serverState;
    Raft raft;
    Interface interface;


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
                                    &clientSocket);
    thread_vector.push_back(std::move(listen_clientThread));

    std::thread listenThread(&Raft::ListeningThread, &raft, &serverSocket, &serverState,
                             &thread_vector, &lk_serverState, &timer);
    thread_vector.push_back(std::move(listenThread));

    while(true){

    }

//    while(true){
//        timer.Atomic_Restart();
//
//        if (serverState.role == FOLLOWER) {
//            while(!timer.Check_Election_timeout()){}    // do nothing
//            /* if election timeout */
//            SetRole_Atomic(&serverState, &lk_serverState, CANDIDATE);
//        }
//
//        else if (serverState.role == CANDIDATE) {
//            Candidate_Role(&serverState, &nodeInfo, &PeerServerInfo,
//                           &thread_vector, &raft, &lk_serverState);
//        }
//
//        else if (serverState.role == LEADER) {
//
//            for (int i = 0; i < nodeInfo.num_peers; i++){
//                std::thread leader_thread(&Raft::LeaderThread, &raft, i, &PeerServerInfo,
//                                          &nodeInfo,&serverState, &lk_serverState);
//                thread_vector.push_back(std::move(leader_thread));
//            }
//
//            while (true) {
//                // just so that we do not spin infinite number of threads
//            }
//        }
//
//        else {
//            std::cout << "Undefined Server Role Initialization" << '\n';
//        }
//    }

}  // End: Main

/* -------------------------------End: Main Function------------------------------------ */

/* ------------------Candidate Helper Functions  ------------------*/
void NewElection_Atomic(ServerState *serverState, std::mutex *lk_serverState,
                        NodeInfo *nodeInfo){
    lk_serverState -> lock();
    serverState -> currentTerm ++;
    serverState -> num_votes = 1;
    serverState -> votedFor = nodeInfo -> node_id; // vote for itself
    lk_serverState -> unlock();
}



void Candidate_Role(ServerState *serverState, NodeInfo *nodeInfo,
                    std::vector<Peer_Info> *PeerServerInfo,
                    std::vector <std::thread> *thread_vector,
                    Raft *raft, std::mutex *lk_serverState){

    int half_peers = nodeInfo -> num_peers / 2;
    int num_votes;
    ServerTimer _timer;

    NewElection_Atomic(serverState, lk_serverState, nodeInfo);

    for (int i = 0; i < nodeInfo -> num_peers; i++){
        std::thread candidate_thread(&Raft::CandidateThread, raft, i, PeerServerInfo,
                                     nodeInfo, serverState, lk_serverState);
        thread_vector -> push_back(std::move(candidate_thread));
    }

    _timer.Start();

    while(!_timer.Check_Election_timeout()){

        lk_serverState -> lock();   // lock
        num_votes = serverState -> num_votes;
        lk_serverState -> unlock();   // unlock

        if (num_votes > half_peers){
            SetRole_Atomic(serverState, lk_serverState, LEADER);
            break;
        }
    }

    lk_serverState -> lock();   // lock
    if (serverState -> role != LEADER) {
        std::cout << "Candidate timeout: resigning to be a follower" << '\n';
        serverState -> role = FOLLOWER;
    }
    lk_serverState -> unlock();   // unlock

}