#include <iostream>
#include <map>

#include "Messages.h"
#include "ClientThread.h"
#include "ClientStub.h"

void Print(std::vector <Peer_Info> *PeerServerInfo){
    Peer_Info peer_server_info;
    for (int i = 0; i < PeerServerInfo -> size(); i++){
        peer_server_info = PeerServerInfo -> at(i);
        std::cout << "id: " << peer_server_info.unique_id <<'\n';
        std::cout << "IP address: " << peer_server_info.IP <<'\n';
        std::cout << "port: " << peer_server_info.port <<'\n';
        std::cout << "" <<'\n';
    }
}

/* return 0 on failure and 1 on success */
int FillPeerServerInfo(int argc, char *argv[], std::vector <Peer_Info> *PeerServerInfo,
                       std::map<int,int> *PeerIdIndexMap){

    int num_servers = atoi(argv[1]);

    for (int i = 0; i < num_servers; i++){

        if (argc < 3*i + 5){
            std::cout << "not enough arguments" << std::endl;
            std::cout << "./client [# num_servers] (repeat [ID] [IP] [port #])" << '\n';
            return 0;
        }

        else{
            int unique_id = atoi(argv[3*i + 2]);
            std::string IP = argv[3*i + 3];
            int server_port = atoi(argv[3*i + 4]);

            Peer_Info peer_server_info {unique_id, IP, server_port};
            PeerServerInfo -> push_back(peer_server_info);
            (*PeerIdIndexMap)[unique_id] = i;
        }
    } // END: for loop

    // Print(PeerServerInfo);
        
    return 1;
}


