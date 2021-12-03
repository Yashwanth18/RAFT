#include "ClientMain.h"

int main(int argc, char *argv[]) {
    std::vector<Peer_Info> PeerServerInfo;


    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo))    { return 0; }

    ClientStub stub;
    int socket_status;
    socket_status = stub.Init(PeerServerInfo.at(0).IP, PeerServerInfo.at(0).port);
    std::cout << "socket_status: " << socket_status << '\n';

}
