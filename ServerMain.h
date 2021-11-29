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
/**
 * Initializes the node info details - role, server_port, client_port, node_id, and num of peers
 * @param nodeInfo the struct containing the node information
 * @param argc number of command line arguments
 * @param argv an array consisting of command line arguments
 * @return 1 if successfully initialized else return 0
 */
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
    nodeInfo -> num_votes = 0;
  return 1;
}

/**
 * Initializes the server state struct which has details about, current term, the id it has
 * voted for, a vector of log, commit index, last_applied log, and match_index array next_index
 * array
 * @param serverState struct consisting the details about the servers log,
 *                    the current term of the server
 * @param num_peers number of peers in the network
 */
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

/**
 * Initializes the socket for different peers
 * @param serverStub a server stub object
 * @param num_peers number of peers in the network
 * @param Socket an array of sockets for different peers
 * @param Is_Init a boolean array to check if a socket is initialized
 * @param Socket_Status a boolean array to check the status of the socket, i.e., if its alive
 */
void Init_Socket(ServerStub * serverStub, int num_peers, int *Socket, bool *Is_Init,
                 bool *Socket_Status){

    for (int i = 0; i < num_peers; i++) {   /* Init Socket */
        Socket[i] = serverStub -> Create_Socket();
        Is_Init[i] = false;
        Socket_Status[i] = false;
    }
}

/* return 0 on failure and 1 on success */

/**
 * Fills the peer server information from the command line arguments
 * @param argc number of command line arguments
 * @param argv array consisting of command line arguments
 * @param PeerServerInfo a vector which stores the information about peer server information
 * @param PeerIdIndexMap a map where key is the peer server id and value is its index in the
 *                       list of peers
 * @return 1 if it successfully initializes, else 0
 */
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

            Peer_Info peer_server_info {unique_id, IP, server_port};
            PeerServerInfo -> push_back(peer_server_info);
            (*PeerIdIndexMap)[unique_id] = i-1;
        }

    } // END: for loop
    return 1;
}

/**
 * Connects with all the peers in the network and updates the socket status for different sockets
 * @param nodeInfo the struct containing the node information
 * @param serverStub a server stub object
 * @param PeerServerInfo a vector which stores the information about peer server information
 * @param Socket an array of sockets for different peers
 * @param Is_Init a boolean array to check if a socket is initialized
 * @param Socket_Status a boolean array to check the status of the socket, i.e., if its alive
 */
void Try_Connect(NodeInfo * nodeInfo, ServerStub * serverStub, std::vector<Peer_Info> *PeerServerInfo,
                 int * Socket, bool * Is_Init, bool * Socket_Status){

    int connect_status;

    for (int i = 0; i < nodeInfo -> num_peers; i++) {       /* iterator through all peers */


        /*  if we have not heard back from ith peer and socket for ith peer is not initialized */
        if (!Is_Init[i]) {
            std::cout << "Still trying to connect "<< '\n';

            connect_status = serverStub -> Connect_To( (*PeerServerInfo) [i].IP,
                                                       (*PeerServerInfo) [i].port, Socket[i]);
            if (connect_status) {   /* connection successful */
                std::cout << "Connection Successful "<< '\n';
                Is_Init[i] = true;
                Socket_Status[i] = true;
                serverStub -> Add_Socket_To_Poll(Socket[i]);
            }
            else {    /* fail connect */
                close (Socket_Status[i]);
                Socket[i] = serverStub -> Create_Socket();
            }

        } /* End: if we have not heard back from ith peer and socket for ith peer is not initialized */
    }  /* End: iterator through all peers */
}


/**
 * It broadcasts append entry request to all the followers, if theres no new log entry to be
 * appended, it sends a heartbeat message as an append entry request to all the followers
 * @param serverState struct consisting the details about the servers log,
 *                    the current term of the server
 * @param nodeInfo the struct containing the node information
 * @param serverStub a server stub object
 * @param Socket an array of sockets for different peers
 * @param Is_Init a boolean array to check if a socket is initialized
 * @param Socket_Status a boolean array to check the status of the socket, i.e., if its alive
 * @param RequestID an array consisting of peer id
 * @param heartbeat a boolean variable set to -1 if its a heartbeat
 */
void BroadCast_AppendEntryRequest(ServerState *serverState, NodeInfo *nodeInfo,
                                  ServerStub *serverStub, int *Socket, bool *Is_Init,
                                  bool *Socket_Status, int *RequestID, bool heartbeat){

    int send_status;
    for (int i = 0; i < nodeInfo -> num_peers; i++) { /* Send to all peers in parallel */

        if (Socket_Status[i]) { /*  if socket is alive */

            if (heartbeat){
                std::cout << "sending heartbeat "<< '\n';
                send_status = serverStub -> SendAppendEntryRequest(serverState, nodeInfo,
                                                                   Socket[i], i, -1);
            }

            else{
                send_status = serverStub -> SendAppendEntryRequest(serverState, nodeInfo,
                                                                   Socket[i], i, RequestID[i]);
            }

            if (!send_status) {
                std::cout << " Fail to send to serve "<< '\n';
                Is_Init[i] = false;
                Socket_Status[i] = false;
                close(Socket[i]);
                Socket[i] = serverStub -> Create_Socket(); // new socket
            }

        } /*  End: if socket is alive */
    }  /* End: Send to all peers in parallel */
}

/**
 * Receives the data from the peers if there data that is to be read
 * @param serverState struct consisting the details about the servers log,
 *                    the current term of the server
 * @param Poll_timeout The timeout argument specifies the number of milliseconds that
 *                     poll() should block waiting for a file descriptor to become ready
 * @param serverStub a server stub object
 * @param PeerIdIndexMap a map where key is the peer server id and value is its index in the
 *                       list of peers
 * @param RequestID an array consisting of peer id
 */
void Get_Ack(ServerState *serverState, int Poll_timeout,
             ServerStub * serverStub, std::map<int,int> *PeerIdIndexMap,
             int *RequestID){

    int poll_count = serverStub -> Poll(Poll_timeout);

    if (poll_count > 0){
        serverStub -> Handle_Poll_Leader(serverState, PeerIdIndexMap, RequestID);
    }
}


/* ------------------------Candidate helper function -----------------------*/

/**
 * Sets up the new election when the node becomes a candidate
 * @param serverState struct consisting the details about the servers log,
 *                    the current term of the server
 * @param timer a timer object which gets restarted at the time of election
 * @param nodeInfo the struct containing the node information
 * @param Request_Completed a boolean array to represent if a request from a peer is completed
 *                          or not
 */
void Setup_New_Election(ServerState * serverState, ServerTimer * timer, NodeInfo *nodeInfo, bool * Request_Completed){

    timer -> Restart(); /* (re)start a new election */
    serverState -> currentTerm ++;
    nodeInfo -> num_votes = 1; // vote for itself

    for (int i = 0; i < nodeInfo -> num_peers; i++){
        Request_Completed[i] = false;
    }
}

/**
 * It broadcasts the request for a vote to all the peers
 * @param serverState struct consisting the details about the servers log,
 *                    the current term of the server
 * @param nodeInfo the struct containing the node information
 * @param serverStub a server stub object
 * @param Socket an array of sockets for different peers
 * @param Is_Init a boolean array to check if a socket is initialized
 * @param Socket_Status a boolean array to check the status of the socket, i.e., if its alive
 * @param Request_Completed a boolean array to represent if a request from a peer is completed
 *                          or not
 */
void BroadCast_RequestVote(ServerState *serverState, NodeInfo * nodeInfo, ServerStub * serverStub, int * Socket,
                           bool * Is_Init, bool * Socket_Status, bool * Request_Completed){

    for (int i = 0; i < nodeInfo -> num_peers; i++) { /* Send to all peers in parallel */

        /*  if we have not heard back from ith peer and socket for ith peer is still alive */
        if (!Request_Completed[i] && Socket_Status[i] ) {

            if (!serverStub -> SendRequestVote(serverState, nodeInfo, Socket[i])) {
                // if send fail
                Is_Init[i] = false;
                Socket_Status[i] = false;
                close(Socket[i]);
                Socket[i] = serverStub->Create_Socket(); // new socket
            }   // End: if send fail

        } /*  End: if we have not heard back from ith peer and socket for ith peer is still alive */
    }  /* End: Send to all peers in parallel */
}


/**
 * It gets the vote from the followers and if the number of votes exceeds the majority, it would
 * become a leader
 * @param serverState struct consisting the details about the servers log,
 *                    the current term of the server
 * @param poll_timeout The timeout argument specifies the number of milliseconds that
 *                     poll() should block waiting for a file descriptor to become ready
 * @param nodeInfo the struct containing the node information
 * @param serverStub a server stub object
 * @param Request_Completed a boolean array to represent if a request from a peer is completed
 *                          or not
 * @param PeerIdIndexMap a map where key is the peer server id and value is its index in the
 *                       list of peers
 */
void Get_Vote(ServerState * serverState, int poll_timeout, NodeInfo * nodeInfo,
              ServerStub * serverStub, bool *Request_Completed, std::map<int,int> *PeerIdIndexMap){


    int poll_count = serverStub -> Poll(poll_timeout);
    int num_votes;
    int majority = nodeInfo -> num_peers / 2;

    if (poll_count > 0){
        serverStub -> Handle_Poll_Candidate(serverState, PeerIdIndexMap, Request_Completed, nodeInfo);

        num_votes = nodeInfo -> num_votes;
        std:: cout << "num_votes: " <<  num_votes << '\n';

        if ( num_votes > majority ){
            nodeInfo -> role = LEADER;
            std::cout << "I'm the leader!" << '\n';
        }
    }
}

/* To be used in the Candidate_Role function. Candidate send a heartbeat message right after being elected
 * to establish its authority to all nodes */

/**
 * Sends a heartbeat to all the followers once the candidate becomes the leader
 * @param serverState struct consisting the details about the servers log,
 *                    the current term of the server
 * @param nodeInfo the struct containing the node information
 * @param serverStub a server stub object
 * @param timer the timer object
 * @param PeerServerInfo a vector which stores the information about peer server information
 * @param PeerIdIndexMap a map where key is the peer server id and value is its index in the
 *                       list of peers
 * @param Is_Init a boolean array to check if a socket is initialized
 * @param Socket_Status a boolean array to check the status of the socket, i.e., if its alive
 * @param Socket an array of sockets for different peers
 */
void Send_One_HeartBeat(ServerState *serverState, NodeInfo *nodeInfo, ServerStub *serverStub,
                        ServerTimer *timer, std::vector<Peer_Info> *PeerServerInfo,
                        std::map<int,int> *PeerIdIndexMap, bool *Is_Init,
                        bool *Socket_Status, int *Socket){

    bool heartbeat = true;
    int RequestID = -1;
    int poll_timeout = timer -> Poll_timeout();

    Try_Connect(nodeInfo, serverStub, PeerServerInfo, Socket, Is_Init, Socket_Status);

    BroadCast_AppendEntryRequest(serverState, nodeInfo, serverStub, Socket, Is_Init,
                                 Socket_Status, &RequestID, heartbeat);

    Get_Ack(serverState, poll_timeout, serverStub, PeerIdIndexMap, &RequestID);
}

/* -------------------------Functions declaration-----------------------------*/

/**
 * If the node role is follower, it calls the specific functions related to functionality of the
 * follower
 * @param serverStub  a server stub object
 * @param serverState a server state object
 * @param timer the timer object
 * @param nodeInfo the struct containing the node information
 */
void Follower_Role(ServerStub *serverStub, ServerState *serverState,
                   ServerTimer *timer, NodeInfo *nodeInfo);

/**
 * If the node becomes a leader, it calls the specific functions related to the functionality of
 * the leader
 * @param serverState a server state object
 * @param nodeInfo the struct containing the node information
 * @param serverStub a server stub object
 * @param poll_timeout The timeout argument specifies the number of milliseconds that
 *                     poll() should block waiting for a file descriptor to become ready
 * @param PeerServerInfo a vector which stores the information about peer server information
 * @param PeerIdIndexMap a map where key is the peer server id and value is its index in the
 *                       list of peers
 * @param Is_Init a boolean array to check if a socket is initialized
 * @param Socket_Status a boolean array to check the status of the socket, i.e., if its alive
 * @param Socket an array of sockets for different peers
 * @param RequestID an array consisting of peer id
 */
void Leader_Role (ServerState *serverState, NodeInfo *nodeInfo, ServerStub *serverStub,
                  int poll_timeout, std::vector<Peer_Info> *PeerServerInfo,
                  std::map<int,int> *PeerIdIndexMap, bool *Is_Init,
                  bool *Socket_Status, int *Socket, int *RequestID);

/**
 * If the node becomes a candidate, it calls the specific functions related to the functionality
 * of the candidate
 * @param serverState a server state object
 * @param nodeInfo the struct containing the node information
 * @param serverStub a server stub object
 * @param timer the timer object
 * @param PeerServerInfo a vector which stores the information about peer server information
 * @param PeerIdIndexMap a map where key is the peer server id and value is its index in the
 *                       list of peers
 * @param Is_Init a boolean array to check if a socket is initialized
 * @param Socket_Status a boolean array to check the status of the socket, i.e., if its alive
 * @param Socket an array consisting of peer id
 */
void Candidate_Role(ServerState *serverState, NodeInfo *nodeInfo, ServerStub *serverStub,
                    ServerTimer *timer, std::vector<Peer_Info> *PeerServerInfo,
                    std::map<int,int> *PeerIdIndexMap, bool *Is_Init,
                    bool *Socket_Status, int *Socket);
