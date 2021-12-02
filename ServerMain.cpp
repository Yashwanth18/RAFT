#include "ServerMain.h"

int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo nodeInfo;
    ServerState serverState;
    ServerSocket serverSocket;
    std::vector<Peer_Info> PeerServerInfo;
    std::vector <std::thread> thread_vector;
    std::mutex lk_serverState;
    Raft raft;

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo))           { return 0; }
    if (!Init_NodeInfo(&nodeInfo, argc, argv))                      { return 0; }
    Init_ServerState(&serverState, nodeInfo.num_peers, argc, argv);

    /* init a listening port to listen to peer servers */
    if (!serverSocket.Init(nodeInfo.server_port)) {
        std::cout << "Socket initialization failed" << std::endl;
        return 0;
    }


    while(true){
        timer.Start();

        if (serverState.role == FOLLOWER) {
            std::thread listenThread(&Raft::ListeningThread, &raft, &serverSocket,
                                     &serverState, &thread_vector, &lk_serverState,
                                     &timer);

            thread_vector.push_back(std::move(listenThread));

            while(!timer.Check_Election_timeout()){

            }

            /* election timeout */
            lk_serverState.lock();          // lock
            serverState.role = CANDIDATE;
            std::cout << "\nI'm a candidate now!" << '\n';
            lk_serverState.unlock();        // unlock

        }

        else if (serverState.role == CANDIDATE) {
            Candidate_Role(&serverState, &nodeInfo, &PeerServerInfo,
                           &thread_vector, &raft, &lk_serverState);
        }

        else if (serverState.role == LEADER) {

            for (int i = 0; i < nodeInfo.num_peers; i++){
                std::thread leader_thread(&Raft::LeaderThread, &raft, i, &PeerServerInfo,
                                          &nodeInfo,&serverState);
                thread_vector.push_back(std::move(leader_thread));
            }

            while (true) {
                // just so that we do not spin infinite number of threads
            }
        }

        else {
            std::cout << "Undefined Server Role Initialization" << '\n';
        }
    }
}

/* -------------------------------End: Main Function------------------------------------ */

/* ------------------Candidate Helper Functions  ------------------*/
void Candidate_Role(ServerState *serverState, NodeInfo *nodeInfo,
                    std::vector<Peer_Info> *PeerServerInfo,
                    std::vector <std::thread> *thread_vector,
                    Raft *raft, std::mutex *lk_serverState){

    int half_peers = nodeInfo -> num_peers / 2;
    int num_votes;
    ServerTimer _timer;

    lk_serverState -> lock();
    serverState -> currentTerm ++;
    std::cout << "currentTerm: " << serverState -> currentTerm << '\n';
    lk_serverState -> unlock();



    for (int i = 0; i < nodeInfo -> num_peers; i++){
        std::thread candidate_thread(&Raft::CandidateThread, raft, i, PeerServerInfo,
                                     nodeInfo, serverState, lk_serverState);
        thread_vector -> push_back(std::move(candidate_thread));
    }

    _timer.Start();

    while(!_timer.Check_Election_timeout()){
        lk_serverState -> lock();   // lock
        num_votes = serverState -> num_votes;
        lk_serverState -> unlock();   // lock

        if (num_votes > half_peers){
            lk_serverState -> lock();   // lock
            serverState -> role = LEADER;
            std::cout << "I'm a leader now!" << '\n';
            lk_serverState -> unlock();   // lock

            break;
        }
    }

    if (serverState -> role != LEADER) {
        std::cout << "Candidate timeout: resigning to be a follower" << '\n';
        serverState -> role = FOLLOWER;
    }
}