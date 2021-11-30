#include <thread>

#include "ServerMain.h"
#include "ServerSocket.h"
#include "ServerAdminStub.h"
#include "ServerThread.h"


int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo nodeInfo;
    ServerState serverState;

    std::vector<Peer_Info> PeerServerInfo;
    std::map <int,int> PeerIdIndexMap;

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo, &PeerIdIndexMap))      { return 0; }
    if (!Init_NodeInfo(&nodeInfo, argc, argv))                                  { return 0; }
    Init_ServerState(&serverState, nodeInfo.num_peers);

    ServerSocket serverSocket;
    std::unique_ptr<ServerSocket> new_socket;
    bool Socket_Status[nodeInfo.num_peers];

    std::vector<std::thread> thread_vector;
    Election election;

    timer.Start();

    if (nodeInfo.role == FOLLOWER) {

        if (!serverSocket.Init(nodeInfo.server_port)) {
            std::cout << "Socket initialization failed" << std::endl;
            return 0;
        }

        while (true) {
            new_socket = serverSocket.Accept();
            std::cout << "Accepted Connection " << '\n';

            std::thread follower_thread(&Election::FollowerThread, &election,
                                        std::move(new_socket));

            thread_vector.push_back(std::move(follower_thread));    // why? Question
        }
        return 0;

    }

    else if (nodeInfo.role == CANDIDATE) {
        ServerAdminStub admin_stub[nodeInfo.num_peers];

        Socket_Status[0] = admin_stub[0].Init(PeerServerInfo[0].IP, PeerServerInfo[0].port);
        std::cout << "Socket_Status: " << Socket_Status[0] << '\n';

        Socket_Status[0] = admin_stub[0].Send_RequestVote(&serverState, &nodeInfo);
        std::cout << "Socket_Status: " << Socket_Status[0] << '\n';
    }

    else if (nodeInfo.role == LEADER) {

    }

    else {
        std::cout << "Undefined Server Role Initialization" << '\n';
    }

}

/* -------------------------------End: Main Function------------------------------------ */


