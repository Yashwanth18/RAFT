#include "ServerMain.h"



int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo nodeInfo;
    ServerState serverState;
    ServerSocket serverSocket;
    std::vector<Peer_Info> PeerServerInfo;

    std::vector <std::thread> thread_vector;
    Raft raft;

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo))           { return 0; }
    if (!Init_NodeInfo(&nodeInfo, argc, argv))                      { return 0; }
    Init_ServerState(&serverState, nodeInfo.num_peers, argc, argv);

    // Each server has a listening port for peer servers
    if (!serverSocket.Init(nodeInfo.server_port)) {
        std::cout << "Socket initialization failed" << std::endl;
        return 0;
    }

    if (serverState.role == LEADER) { // for testing log replication
      Read_Logs_From_File(&serverState);

      for (int i = 0; i < nodeInfo.num_peers ; i++){

        serverState.nextIndex.push_back(serverState.smr_log.size()-1);
      }
      //std::cout << "server state next index size is : " << serverState.nextIndex[0] << '\n';
    }

    while(true){

        if (serverState.role == FOLLOWER) {
            /* log replication: read from external file 2 */


            std::this_thread::sleep_for (std::chrono::seconds(1));

            timer.Start();
            std::thread follower_listenThread(&Raft::Follower_ListeningThread, &raft,
                                      &serverSocket, &serverState,
                                      &thread_vector, &timer);

            thread_vector.push_back(std::move(follower_listenThread));

            while (true) {
//                if (timer.Check_Election_timeout()){
//                    serverState.role = CANDIDATE;
//                    std::cout << "I'm a candidate now!" << '\n';
//                    break;
//                }
//                timer.Print_elapsed_time();
            }
        }

        else if (serverState.role == CANDIDATE) {
            Candidate_Role(&serverState, &nodeInfo, &PeerServerInfo, &thread_vector, &raft);
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

void Candidate_Role(ServerState *serverState, NodeInfo *nodeInfo,
                    std::vector<Peer_Info> *PeerServerInfo,
                    std::vector <std::thread> *thread_vector, Raft *raft){

    int num_votes;
    int majority;
    std::mutex lk_State;

    serverState -> currentTerm ++;
    majority = nodeInfo -> num_peers / 2;


    for (int i = 0; i < nodeInfo -> num_peers; i++){
        std::thread candidate_thread(&Raft::CandidateThread, raft,
                                     i, PeerServerInfo, nodeInfo,
                                     serverState);

        thread_vector -> push_back(std::move(candidate_thread));
    }

    while (true) {
        lk_State.lock();
        num_votes = serverState -> num_votes;
        std::cout << "num_votes: " << num_votes << '\n';
        lk_State.unlock();

        if (num_votes > majority){
            serverState -> role = LEADER;

            // initialize next index
            for (int i = 0; i < nodeInfo->num_peers; i++){

              serverState -> nextIndex.push_back(serverState -> smr_log.size() - 1); // initializing next index to be last
                                                                               // log index of leader
            }

            std::cout << "I'm a leader now!" << '\n';
            break;
        }
    }
}