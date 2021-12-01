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
        return 0;

    }

    else if (serverState.role == CANDIDATE) {
      int sent  = false;
        while(true){
            if (!sent){
              ServerOutStub Out_stub;

              std::string peer_IP = PeerServerInfo[0].IP;
              int peer_port = PeerServerInfo[0].port;

              Out_stub.Init(peer_IP, peer_port);
              int send_status = Out_stub.Send_RequestVote(&serverState, &nodeInfo);
              std::cout<< "send_status: " << send_status << '\n';

              if (send_status > 0){
                sent = true;
                char buf[1];
                Out_stub.Read(buf, 1);
              }
            }
        }
    }

    else if (serverState.role == LEADER) {

    }

    else {
        std::cout << "Undefined Server Role Initialization" << '\n';
    }

}

/* -------------------------------End: Main Function------------------------------------ */
