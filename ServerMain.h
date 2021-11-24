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

/* Command line argument format:
    ./server port_server port_client nodeID num_peers (repeat PeerID IP port_server)
*/

/* return 0 on failure and 1 on success */
int Init_NodeInfo(NodeInfo * nodeInfo, int argc, char *argv[]){
    if (argc < 4){
        std::cout << "not enough arguments" << std::endl;
        return 0;
    }

//    nodeInfo -> role = FOLLOWER;
    nodeInfo -> role = atoi(argv[argc - 1]);  /* for testing purpose only! */

    nodeInfo -> leader_id = -1;

    nodeInfo -> server_port = atoi(argv[1]);
    nodeInfo -> client_port = atoi(argv[2]);
    nodeInfo -> node_id = atoi(argv[3]);
    nodeInfo -> num_peers = atoi(argv[4]);
    return 1;
}

void Init_ServerState(ServerState * serverState, int num_peers){
    /* Persistent state on all servers: Updated on stable storage before responding to RPCs */
    serverState -> currentTerm = 0;
    serverState -> votedFor = -1;
    LogEntry logEntry {-1, -1, -1, -1};
    serverState -> smr_log.push_back(logEntry);


    /* volatile state on all servers */
    serverState -> commitIndex = 0;
    serverState -> last_applied = 0;

    /* volatile state on leaders (Reinitialized after election) */

    for (int i = 0; i < num_peers; i++){
        serverState -> matchIndex.push_back(0);
        serverState -> nextIndex.push_back(1);
    }
}

/* return 0 on failure and 1 on success */
int FillPeerServerInfo(int argc, char *argv[], std::vector<Peer_Info> *PeerServerInfo,
                       std::map<int,int> *PeerIdIndexMap){

    int num_peers = atoi(argv[4]);

    for (int i = 1; i <= num_peers; i++){

        if (argc <= 3*i + 4){
            std::cout << "not enough arguments" << std::endl;
            std::cout << "./server [port_server #] [port_client #] [unique ID] [# peers] "
                         "(repeat [ID] [IP] [port #])	" << std::endl;
            return 0;
        }

        else{

            int unique_id = atoi(argv[3*i + 2]);
            std::string IP = argv[3*i + 3];
            int server_port = atoi(argv[3*i + 4]);

            /*
            std::cout << "Peer_id: " << unique_id << '\n';
            std::cout << "Ip: " << IP << '\n';
            std::cout << "server_port: " << server_port << '\n';
            */

            Peer_Info peer_server_info {unique_id, IP, server_port};
            PeerServerInfo -> push_back(peer_server_info);
            (*PeerIdIndexMap)[unique_id] = i-1;
        }

    } // END: for loop
    return 1;
}

void BroadCast_Request_Vote(ServerState *serverState, NodeInfo * nodeInfo, ServerStub * serverStub, int * Socket,
                            bool * Is_Init, bool * Socket_Status, bool * Request_Completed){

    for (int i = 0; i < nodeInfo->num_peers; i++) { /* Send to all peers in parallel */

        /*  if we have not heard back from ith peer and socket for ith peer is still alive */
        if (!Request_Completed[i] && Socket_Status[i] ) {

            if (!serverStub -> SendRequestVote(serverState, nodeInfo, Socket[i])) { // if send fail
                Is_Init[i] = false;
                Socket_Status[i] = false;
                close(Socket[i]);
                Socket[i] = serverStub->Create_Socket(); // new socket
            }   // End: if send fail

        } /*  End: if we have not heard back from ith peer and socket for ith peer is still alive */
    }  /* End: Send to all peers in parallel */
}

void Setup_New_Election(ServerState * serverState, ServerTimer * timer,
                        NodeInfo *nodeInfo, bool * Request_Completed){

    timer -> Restart(); /* (re)start a new election */
    serverState -> currentTerm ++;
    nodeInfo -> num_votes = 1; // vote for itself

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

void Get_Vote(ServerState * serverState, ServerTimer * timer,
              NodeInfo * nodeInfo, ServerStub * serverStub,
              bool *Request_Completed, std::map<int,int> *PeerIdIndexMap){

    int Poll_timeout = timer -> Poll_timeout();
    int poll_count = serverStub -> Poll(Poll_timeout);
    int num_votes;
    int majority = nodeInfo -> num_peers / 2;

    if (poll_count > 0){
        serverStub -> Handle_Poll_Peer(serverState, PeerIdIndexMap, Request_Completed,
                                       nodeInfo);

        num_votes = nodeInfo -> num_votes;
        std:: cout << "num_votes: " <<  num_votes << '\n';

        if ( num_votes > majority ){
            nodeInfo -> role = LEADER;
            std::cout << "I'm the leader!" << '\n';
        }
    }
}

void Init_Socket(ServerStub * serverStub, int num_peers, int *Socket, bool *Is_Init,
                 bool *Socket_Status){

    for (int i = 0; i < num_peers; i++) {   /* Init Socket */
        Socket[i] = serverStub -> Create_Socket();
        Is_Init[i] = false;
        Socket_Status[i] = false;
    }
}

