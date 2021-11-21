#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <iomanip>
#include <map>

#include "Messages.h"
#include "ServerTimer.h"
#include "ServerStub.h"

//return 0 on failure and 1 on success
int Init_NodeInfo(NodeInfo * nodeInfo, int argc, char *argv[]){
    if (argc < 4){
        std::cout << "not enough arguments" << std::endl;
        return 0;
    }

    nodeInfo -> role = FOLLOWER;
    nodeInfo -> leader_id = -1;
    nodeInfo -> node_id = atoi(argv[2]);

    nodeInfo -> port = atoi(argv[1]);
    nodeInfo -> num_peers = atoi(argv[3]);

    /* Used in RequestVote*/
    nodeInfo -> term = 0;
    nodeInfo ->  votedFor = -1;

     /* change this to a real vector of struct log */
    nodeInfo ->  lastLogTerm = 0;
    nodeInfo ->  lastLogIndex = 0;

  return 1;
}

/* return 0 on failure and 1 on success */
int FillPeerServerInfo(int argc, char *argv[], std::vector<Peer_Info> *PeerServerInfo,
                       std::map<int,int> *PeerIdIndexMap){

    int num_peers = atoi(argv[3]);

    for (int i = 1; i <= num_peers; i++){

        if (argc <= 3*i + 3){
            std::cout << "not enough arguments" << std::endl;
            std::cout << "./server [port #] [unique ID] [# peers] "
                         "(repeat [ID] [IP] [port #])	" << std::endl;
            return 0;
        }

        else{
            int unique_id = atoi(argv[3*i + 1]);
            std::string IP = argv[3*i + 2];
            int port = atoi(argv[3*i + 3]);

            Peer_Info peer_server_info {unique_id, IP, port};
            PeerServerInfo -> push_back(peer_server_info);
            (*PeerIdIndexMap)[unique_id] = i-1;
        }

    } //END for loop
    return 1;
}

void BroadCast_Request_Vote(NodeInfo * nodeInfo, ServerStub * serverStub, int * Socket,
                            bool * Is_Init, bool * Socket_Status, bool * Request_Completed){

    for (int i = 0; i < nodeInfo->num_peers; i++) { /* Send to all peers in parallel */

        /*  if we have not heard back from ith peer and socket for ith peer is still alive */
        if (!Request_Completed[i] && Socket_Status[i] ) {

            if (!serverStub -> SendRequestVote(nodeInfo, Socket[i])) { // if send fail
                Is_Init[i] = false;
                Socket_Status[i] = false;
                close(Socket[i]);
                Socket[i] = serverStub->Create_Socket(); // new socket
            }   // End: if send fail

        } /*  End: if we have not heard back from ith peer and socket for ith peer is still alive */
    }  /* End: Send to all peers in parallel */
}

void Setup_New_Election(ServerTimer * timer, int * num_votes,
                        NodeInfo *nodeInfo, bool * Request_Completed){

    timer -> Restart(); /* (re)start a new election */
    (*num_votes) = 0;
    nodeInfo -> term ++;

    for (int i = 0; i < nodeInfo -> num_peers; i++){
        Request_Completed[i] = false;
    }
}

void Try_Connect(NodeInfo * nodeInfo, ServerStub * serverStub, std::vector<Peer_Info> *PeerServerInfo,
                 int * Socket, bool * Is_Init, bool * Socket_Status, bool * Request_Completed){

    int connect_status;
    for (int i = 0; i < nodeInfo -> num_peers; i++) {       /* iterator through all peers */

        /*  if we have not heard back from ith peer and socket for ith peer is not initialized */
        if (!Request_Completed[i] && !Is_Init[i]) {

            connect_status = serverStub -> Connect_To( (*PeerServerInfo) [i].IP,
                                                    (*PeerServerInfo) [i].port, Socket[i]);

            if (connect_status) {   /* connection successful */
                Is_Init[i] = true;
                Socket_Status[i] = true;
                serverStub -> Add_Socket_To_Poll(Socket[i]);
            }
            else {                  /* fail connect */
                close(Socket_Status[i]);
                Socket[i] = serverStub -> Create_Socket();
            }

        } /* End: if we have not heard back from ith peer and socket for ith peer is not initialized */
    }  /* End: iterator through all peers */
}

void Get_Vote(ServerTimer * timer, NodeInfo * nodeInfo, ServerStub * serverStub,
              int * num_votes, bool *Request_Completed, std::map<int,int> *PeerIdIndexMap){

    int Poll_timeout = timer -> Poll_timeout();
    int poll_count = serverStub -> Poll(Poll_timeout);

    if (poll_count > 0){
        serverStub -> Handle_Poll_Peer(PeerIdIndexMap, Request_Completed, num_votes, nodeInfo);

        if ( (*num_votes) > nodeInfo -> num_peers / 2 ){
            nodeInfo -> role = LEADER;
        }

        std::cout << "the role of the node is: " << nodeInfo -> role << '\n';
    }
}

