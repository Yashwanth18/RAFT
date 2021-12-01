#include <thread>

#include "ServerMain.h"
#include "ServerSocket.h"
#include "ServerOutStub.h"
#include "ServerThread.h"


int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo nodeInfo;
    ServerState serverState;

    std::vector<Peer_Info> PeerServerInfo;
    std::map <int,int> PeerIdIndexMap;

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo, &PeerIdIndexMap))      { return 0; }
    if (!Init_NodeInfo(&nodeInfo, argc, argv))                                  { return 0; }
    Init_ServerState(&serverState, nodeInfo.num_peers, argc, argv);

    ServerSocket serverSocket;


    std::vector <std::thread> thread_vector;
    Raft Raft;

    // Each server has a listening port for peer servers
    if (!serverSocket.Init(nodeInfo.server_port)) {
        std::cout << "Socket initialization failed" << std::endl;
        return 0;
    }


    while(true){
        if (serverState.role == FOLLOWER) {
            std::this_thread::sleep_for (std::chrono::seconds(2));

            timer.Start();
            std::thread Listen_thread(&Raft::Follower_ListeningThread, &Raft,
                                      &serverSocket, &nodeInfo, &serverState,
                                      &thread_vector, &timer);

            thread_vector.push_back(std::move(Listen_thread));

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

            serverState.currentTerm ++;
            bool sent[nodeInfo.num_peers];
            for (int i = 0; i < nodeInfo.num_peers; i++){
                sent[i] = false;
            }

            for (int i = 0; i < nodeInfo.num_peers; i++){
                std::thread candidate_thread(&Raft::CandidateThread, &Raft,
                                             0, &PeerServerInfo, &nodeInfo,
                                             &serverState, &sent[i]);

                thread_vector.push_back(std::move(candidate_thread));
            }

            while (true) {

                int num_votes = serverState.num_votes;
                int majority = nodeInfo.num_peers / 2;

                if (num_votes > majority){
                    // serverState.role = LEADER;
                    std::cout << "I'm a leader now!" << '\n';
                    // break;
                }
            }

        }

        else if (serverState.role == LEADER) {
            bool sent[nodeInfo.num_peers];
            for (int i = 0; i < nodeInfo.num_peers; i++){
                sent[i] = false;
            }

            for (int i = 0; i < nodeInfo.num_peers; i++){
                std::thread leader_thread(&Raft::LeaderThread, &Raft,
                                          i, &PeerServerInfo, &nodeInfo,
                                          &serverState, &sent[i]);

                thread_vector.push_back(std::move(leader_thread));
            }
            while (true) {

            }
        }

        else {
            std::cout << "Undefined Server Role Initialization" << '\n';
        }
    }
}

/* -------------------------------End: Main Function------------------------------------ */
