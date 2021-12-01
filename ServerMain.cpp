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
    std::unique_ptr<ServerSocket> new_socket;

    std::vector<std::thread> thread_vector;
    Election election;

    timer.Start();

    if (serverState.role == FOLLOWER) {

        if (!serverSocket.Init(nodeInfo.server_port)) {
            std::cout << "Socket initialization failed" << std::endl;
            return 0;
        }

        while (true) {
            new_socket = serverSocket.Accept();
            std::cout << "Accepted Connection from peer server" << '\n';

            std::thread follower_thread(&Election::FollowerThread, &election,
                                        std::move(new_socket), &nodeInfo, &serverState);

            thread_vector.push_back(std::move(follower_thread));
        }
    }

    else if (serverState.role == CANDIDATE) {
        bool sent = false;
        while (true) {
            std::thread candidate_thread(&Election::CandidateThread, &election,
                                         0, &PeerServerInfo, &nodeInfo,
                                         &serverState, &sent);

            thread_vector.push_back(std::move(candidate_thread));
        }
    }

    else if (serverState.role == LEADER) {
        bool sent = false;
        while (true) {
            std::thread candidate_thread(&Election::CandidateThread, &election,
                                         0, &PeerServerInfo, &nodeInfo,
                                         &serverState, &sent);

            thread_vector.push_back(std::move(candidate_thread));
        }
    }

    else {
        std::cout << "Undefined Server Role Initialization" << '\n';
    }

}

/* -------------------------------End: Main Function------------------------------------ */
