#include "ServerMain.h"


int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo nodeInfo;
    ServerStub serverStub;
    ServerState serverState;

    std::vector<Peer_Info> PeerServerInfo;
    std::map<int,int> PeerIdIndexMap;

    int poll_timeout = timer.Poll_timeout();
    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo, &PeerIdIndexMap))      { return 0; }
    if (!Init_NodeInfo(&nodeInfo, argc, argv))                                  { return 0; }

    Init_ServerState(&serverState, nodeInfo.num_peers);
    serverStub.Init(&nodeInfo);     // a listening port for peer servers is created here

    /* variables for error handling related to Socket*/
    int Socket[nodeInfo.num_peers];
    bool Socket_Status[nodeInfo.num_peers];  /* 0: Dead, 1: Alive */
    bool Is_Init[nodeInfo.num_peers];
    Init_Socket(&serverStub, nodeInfo.num_peers, Socket, Is_Init, Socket_Status);

    int LogRep_RequestID [nodeInfo.num_peers];

    /* Debug purpose only!: make believe for now that this comes from the customer */
    LogEntry logEntry1 {1, 10, 11, 12};
    serverState.smr_log.push_back(logEntry1);
    /* ------------------------------------------------------------------------*/

    if (nodeInfo.role == FOLLOWER){
         std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    timer.Start();
    while(true) {

        if (nodeInfo.role == FOLLOWER) {
            Follower_Role(&serverStub, &serverState, &timer, &nodeInfo, Socket, Is_Init, Socket_Status);
        }

        else if (nodeInfo.role == CANDIDATE) {
            Candidate_Role(&serverState, &nodeInfo, &serverStub, &timer, &PeerServerInfo,
                           &PeerIdIndexMap, Is_Init, Socket_Status, Socket, LogRep_RequestID);
        }

        else if (nodeInfo.role == LEADER) {
            Leader_Role (&serverState, &nodeInfo, &serverStub, poll_timeout, &PeerServerInfo,
                         &PeerIdIndexMap, Is_Init, Socket_Status, Socket, LogRep_RequestID);
        }
        else {
            std::cout << "Undefined Server Role Initialization" << '\n';
        }
    }
}
/* -------------------------------End: Main Function------------------------------------ */

void Leader_Role (ServerState *serverState, NodeInfo *nodeInfo, ServerStub *serverStub,
                 int poll_timeout, std::vector<Peer_Info> *PeerServerInfo,
                 std::map<int,int> *PeerIdIndexMap, bool *Is_Init,
                 bool *Socket_Status, int *Socket, int *LogRep_RequestID){

    /* to-do: merge this with client interface. Wait for the request to arrive in the request queue
     * for some duration. If no request, break wait conditional variable and set heartbeat = true */
    // std::this_thread::sleep_for(std::chrono::milliseconds(poll_timeout / 2));
    bool heartbeat = true;

    Try_Connect(nodeInfo, serverStub, PeerServerInfo, Socket, Is_Init, Socket_Status);

    BroadCast_AppendEntryRequest(serverState, nodeInfo, serverStub, Socket,
                                 Is_Init, Socket_Status, LogRep_RequestID, heartbeat);

    Get_Ack(serverState, nodeInfo, poll_timeout, serverStub, PeerIdIndexMap, LogRep_RequestID);
 }



void Candidate_Role(ServerState *serverState, NodeInfo *nodeInfo, ServerStub *serverStub,
                    ServerTimer *timer, std::vector<Peer_Info> *PeerServerInfo,
                    std::map<int,int> *PeerIdIndexMap, bool *Is_Init,
                    bool *Socket_Status, int *Socket, int *LogRep_RequestID) {

    int poll_timeout = timer -> Poll_timeout();
    bool VoteRequest_Completed [ nodeInfo -> num_peers ];
    bool VoteRequest_Sent [ nodeInfo -> num_peers ];

    // reset timer, increment term number, reset variables
    Setup_New_Election(serverState, timer, nodeInfo, VoteRequest_Completed, VoteRequest_Sent);

    while (nodeInfo -> role == CANDIDATE) {

        if (timer -> Check_election_timeout()){
            std::cout << "Candidate timeout: Candidate Resigning to be a follower "<< '\n';
            nodeInfo -> role = FOLLOWER;     // to-do: for when follower can time out
            break;
        }

        Try_Connect(nodeInfo, serverStub, PeerServerInfo, Socket, Is_Init, Socket_Status);

        BroadCast_VoteRequest(serverState, nodeInfo, serverStub, Socket, Is_Init,
                              Socket_Status, VoteRequest_Sent);

        Get_Vote(serverState, poll_timeout, nodeInfo, serverStub, VoteRequest_Completed,
                 PeerIdIndexMap);

        if (nodeInfo -> role == LEADER) {
            Send_One_HeartBeat(serverState, nodeInfo, serverStub, timer, PeerServerInfo,
                               PeerIdIndexMap, Is_Init, Socket_Status, Socket);

            for (int i = 0; i < nodeInfo -> num_peers; i++) {
                LogRep_RequestID[i] = 0; // reset LogRep_RequestID for log replication
            }
        }
    }

}


void Follower_Role(ServerStub *serverStub, ServerState *serverState, ServerTimer *timer,
                   NodeInfo *nodeInfo, int *Socket, bool *Is_Init, bool *Socket_Status){

    int poll_count;
    int poll_timeout = timer -> Poll_timeout();

//    poll_count = serverStub -> Poll(poll_timeout);
//    if (poll_count > 0) {
//        serverStub -> Handle_Poll_Follower(timer, serverState, nodeInfo);
//    }

    if ( timer -> Check_election_timeout() ) {
        nodeInfo -> role = CANDIDATE;
        Init_Socket(serverStub, nodeInfo -> num_peers, Socket, Is_Init, Socket_Status);

        std::cout << "Timeout: I'm the candidate now!" << '\n';
    }

    else {
        poll_count = serverStub -> Poll(poll_timeout); // listen longer than leader by 2x
        if (poll_count > 0) {
            serverStub -> Handle_Poll_Follower(timer, serverState, nodeInfo);
        }
    }

}


