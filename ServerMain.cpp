#include "ServerMain.h"


int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo nodeInfo;
    ServerState serverState;

    std::vector<Peer_Info> PeerServerInfo;
    std::map <int,int> PeerIdIndexMap;

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo, &PeerIdIndexMap))      { return 0; }
    if (!Init_NodeInfo(&nodeInfo, argc, argv))                                  { return 0; }
    Init_ServerState(&serverState, nodeInfo.num_peers);

    timer.Start();
    if (nodeInfo.role == FOLLOWER) {

    }

    else if (nodeInfo.role == CANDIDATE) {

    }

    else if (nodeInfo.role == LEADER) {

    }

    else {
        std::cout << "Undefined Server Role Initialization" << '\n';
    }

}

/* -------------------------------End: Main Function------------------------------------ */


