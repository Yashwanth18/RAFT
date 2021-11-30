#include "ServerMain.h"
#include "ServerSocket.h"
#include "ServerAdminStub.h"


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

    timer.Start();

    if (nodeInfo.role == FOLLOWER) {

        if (!serverSocket.Init(nodeInfo.server_port)) {
            std::cout << "Socket initialization failed" << std::endl;
            return 0;
        }

        while ( (new_socket = serverSocket.Accept()) ) {
            std::cout << "Accepted Connection " << '\n';
        }
        return 0;

    }

    else if (nodeInfo.role == CANDIDATE) {
        ServerAdminStub admin_stub[nodeInfo.num_peers];
        Socket_Status[0] = admin_stub[0].Init(PeerServerInfo[0].IP, PeerServerInfo[0].port);
    }

    else if (nodeInfo.role == LEADER) {

    }

    else {
        std::cout << "Undefined Server Role Initialization" << '\n';
    }

}

/* -------------------------------End: Main Function------------------------------------ */


