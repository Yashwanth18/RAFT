#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <iomanip>
#include <map>
#include <chrono>
#include <thread>

#include "Messages.h"
#include "ServerTimer.h"
#include "ServerStub.h"

/* Usage: ./server port_server port_client nodeID num_peers (repeat PeerID IP port_server) */

/* return 0 on failure and 1 on success */
int Init_NodeInfo(NodeInfo * nodeInfo, int argc, char *argv[]){
    if (argc < 4){
        std::cout << "not enough arguments" << std::endl;
        return 0;
    }

    nodeInfo -> role = atoi(argv[argc - 1]);    /* for testing purpose only! */

    nodeInfo -> leader_id = -1;
    nodeInfo -> server_port = atoi(argv[1]);
    nodeInfo -> client_port = atoi(argv[2]);
    nodeInfo -> node_id = atoi(argv[3]);
    nodeInfo -> num_peers = atoi(argv[4]);
    nodeInfo -> num_votes = 0;
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

void Init_Socket(ServerStub * serverStub, int num_peers, int *Socket, bool *Is_Init,
                 bool *Socket_Status){

    for (int i = 0; i < num_peers; i++) {   /* Init Socket */
        Socket[i] = serverStub -> Create_Socket();
        Is_Init[i] = false;
        Socket_Status[i] = false;
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

            std::cout << "unique_id: " << unique_id << std::endl;
            std::cout << "IP: " << IP << std::endl;
            std::cout << "server_port: " << server_port << std::endl;

            Peer_Info peer_server_info {unique_id, IP, server_port};
            PeerServerInfo -> push_back(peer_server_info);
            (*PeerIdIndexMap)[unique_id] = i-1;
        }

    } // END: for loop
    return 1;
}

void Try_Connect(NodeInfo * nodeInfo, ServerStub * serverStub, std::vector<Peer_Info> *PeerServerInfo,
                 int * Socket, bool * Is_Init, bool * Socket_Status){

    int connect_status;

    for (int i = 0; i < nodeInfo -> num_peers; i++) {       /* iterator through all peers */

        if (!Is_Init[i]) {
            std::cout << "Trying to Connect"<< '\n';
            connect_status = serverStub -> Connect_To( (*PeerServerInfo) [i].IP,
                                                       (*PeerServerInfo) [i].port, Socket[i]);


            if (connect_status) {   /* connection successful */
                // problem here !!!
                std::cout << "Connection successful"<< '\n';
                Is_Init[i] = true;
                Socket_Status[i] = true;
                serverStub -> Set_Pfd(Socket[i], i);
            }
            else {    /* fail connect */
                Is_Init[i] = false;
                Socket_Status[i] = false;
                close (Socket_Status[i]);
                Socket[i] = serverStub -> Create_Socket();
            }

        } /* End: not initialized */
    }  /* End: iterator through all peers */
}

/* ---------------------------------------------------------------------------------*/
/* ------------------------Candidate helper function -----------------------*/

void Setup_New_Election(ServerState * serverState, ServerTimer * timer,
                        NodeInfo *nodeInfo, bool * VoteRequest_Completed,
                        bool *VoteRequest_Sent){

    serverState -> currentTerm ++;
    nodeInfo -> num_votes = 1; // vote for itself

    for (int i = 0; i < nodeInfo -> num_peers; i++){
        VoteRequest_Completed[i] = false;
        VoteRequest_Sent[i] = false;
    }

    timer -> Restart(); /* (re)start a new election */
}

void BroadCast_VoteRequest(ServerState *serverState, NodeInfo * nodeInfo,
                           ServerStub * serverStub, int * Socket, bool * Is_Init,
                           bool * Socket_Status, bool *VoteRequest_Sent){
    int send_status;
    for (int i = 0; i < nodeInfo -> num_peers; i++) { /* Send to all peers in parallel */

        /*  if we have not sent and socket is alive */
        if (!VoteRequest_Sent[i] && Socket_Status[i] ) {

            send_status = serverStub -> SendVoteRequest(serverState, nodeInfo, Socket[i]);

            if (send_status) {  // send succeed
                VoteRequest_Sent[i] = true;
            }

            else{ // if send fail
                Is_Init[i] = false;
                Socket_Status[i] = false;
                close(Socket[i]);
                Socket[i] = serverStub -> Create_Socket(); // new socket
            }
        } /*  End: if we have not sent and socket is alive */
    }  /* End: Send to all peers in parallel */
}



void Get_Vote(ServerState * serverState, int poll_timeout, NodeInfo * nodeInfo,
              ServerStub * serverStub, bool *VoteRequest_Completed, std::map<int,int> *PeerIdIndexMap,
              int *Socket, bool *Is_Init, bool *Socket_Status){


    int num_votes;
    int majority = nodeInfo -> num_peers / 2;
    int poll_count = serverStub -> Poll(poll_timeout);

    if (poll_count > 0){
        serverStub -> Handle_Poll_Candidate(serverState, PeerIdIndexMap,
                                            VoteRequest_Completed, nodeInfo,
                                            Socket, Is_Init, Socket_Status);

        num_votes = nodeInfo -> num_votes;

        if (num_votes > majority ){
            nodeInfo -> role = LEADER;
            std:: cout << "num_votes: " <<  num_votes << '\n';
            std::cout << "\n****------------------******" << '\n';
            std::cout << "****----I'm the leader!---******" << '\n';
            std::cout << "****--------------------******\n" << '\n';
        }
    }
}


void BroadCast_AppendEntryRequest(ServerState *serverState, NodeInfo *nodeInfo,
                                  ServerStub *serverStub, int *Socket, bool *Is_Init,
                                  bool *Socket_Status, int *LogRep_RequestID, bool heartbeat){

    int send_status;
    for (int i = 0; i < nodeInfo -> num_peers; i++) { /* Send to all peers in parallel */

        if (Socket_Status[i]) { /*  if socket is alive */

            if (heartbeat){
                send_status = serverStub ->
                        SendAppendEntryRequest(serverState, nodeInfo, Socket[i], i, -1);
            }

            else{
                send_status = serverStub ->
                        SendAppendEntryRequest(serverState, nodeInfo, Socket[i], i, LogRep_RequestID[i]);
            }

            if (!send_status) {
                std::cout << "Fail to send "<< '\n';
                Is_Init[i] = false;
                Socket_Status[i] = false;
                close(Socket[i]);
                Socket[i] = serverStub -> Create_Socket(); // new socket
            }

        } /*  End: if socket is alive */
    }  /* End: Send to all peers in parallel */
}

void Get_Ack(ServerState *serverState, NodeInfo *nodeInfo, int Poll_timeout,
             ServerStub * serverStub, std::map<int,int> *PeerIdIndexMap,
             int *LogRep_RequestID, int *Socket, bool *Is_Init, bool *Socket_Status){

    int poll_count = serverStub -> Poll(Poll_timeout);

    if (poll_count > 0){
        serverStub -> Handle_Poll_Leader(serverState, nodeInfo, PeerIdIndexMap,
                                         LogRep_RequestID, Socket, Is_Init, Socket_Status);
    }
}



/* ---------------------------------------------------------------------------*/
/* To be used in the Candidate_Role function. Candidate send a heartbeat message right
 * after being elected to establish its authority to all nodes */
void Send_One_HeartBeat(ServerState *serverState, NodeInfo *nodeInfo, ServerStub *serverStub,
                        ServerTimer *timer, std::vector<Peer_Info> *PeerServerInfo,
                        std::map<int,int> *PeerIdIndexMap, bool *Is_Init,
                        bool *Socket_Status, int *Socket){

    bool heartbeat = true;
    int LogRep_RequestID = -1;
    int poll_timeout = timer -> Poll_timeout();

    Try_Connect(nodeInfo, serverStub, PeerServerInfo, Socket, Is_Init, Socket_Status);

    BroadCast_AppendEntryRequest(serverState, nodeInfo, serverStub, Socket, Is_Init,
                                 Socket_Status, &LogRep_RequestID, heartbeat);

    Get_Ack(serverState, nodeInfo, poll_timeout, serverStub, PeerIdIndexMap,
            &LogRep_RequestID, Socket, Is_Init, Socket_Status);
}

/* -------------------------Functions declaration-----------------------------*/
void Follower_Role(ServerStub *serverStub, ServerState *serverState, ServerTimer *timer,
                   NodeInfo *nodeInfo, int *Socket, bool *Is_Init, bool *Socket_Status);

void Leader_Role (ServerState *serverState, NodeInfo *nodeInfo, ServerStub *serverStub,
                  int poll_timeout, std::vector<Peer_Info> *PeerServerInfo,
                  std::map<int,int> *PeerIdIndexMap, bool *Is_Init,
                  bool *Socket_Status, int *Socket, int *LogRep_RequestID);

void Candidate_Role(ServerState *serverState, NodeInfo *nodeInfo, ServerStub *serverStub,
                    ServerTimer *timer, std::vector<Peer_Info> *PeerServerInfo,
                    std::map<int,int> *PeerIdIndexMap, bool *Is_Init,
                    bool *Socket_Status, int *Socket, int *LogRep_RequestID);
