#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <iomanip>

#include "Messages.h"




//return 0 on failure and 1 on success
int Init_Node_Info(NodeInfo * node_info, int argc, char *argv[]){
    if (argc < 4){
        std::cout << "not enough arguments" << std::endl;
        return 0;
    }

    node_info -> leader_id = -1;
    node_info -> node_id = atoi(argv[2]);

    node_info -> port = atoi(argv[1]);
    node_info -> num_peers = atoi(argv[3]);

    /* Used in RequestVote*/
    node_info -> term = 0;
    node_info ->  votedFor = -1;

     /* change this to a real vector of struct log */
    node_info ->  lastLogTerm = 0;
    node_info ->  lastLogIndex = 0;

  return 1;
}

/* return 0 on failure and 1 on success */
int FillPeerServerInfo(int argc, char *argv[], std::vector<Peer_Info> *PeerServerInfo){

    int num_peers = atoi(argv[3]);

    for (int i = 1; i <= num_peers; i++){

        if (argc <= 3*i + 3){
            std::cout << "not enough arguments" << std::endl;
            std::cout << "./server [port #] [unique ID] [# peers] \
                      (repeat [ID] [IP] [port #])	" << std::endl;
            return 0;
        }

        else{
            int unique_id = atoi(argv[3*i + 1]);
            std::string IP = argv[3*i + 2];
            int port = atoi(argv[3*i + 3]);

            Peer_Info peer_server_info {unique_id, IP, port};
            PeerServerInfo -> push_back(peer_server_info);
        }

    } //END for loop
    return 1;
}

// * For debugging */
//void Print_PeerServerInfo(){
//    for (int i = 0; i < num_peers; i++){
//        std::cout << "id: "<< PeerServerInfo[i].unique_id  << '\n';
//        std::cout << "IP: "<< PeerServerInfo[i].IP  << '\n';
//        std::cout << "Port: "<< PeerServerInfo[i].port  << '\n';
//        std::cout << "------------------"<< '\n';
//    }
//}

