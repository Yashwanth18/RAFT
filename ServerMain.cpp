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

    int RequestID [nodeInfo.num_peers];
    for (int i = 0; i < nodeInfo.num_peers; i++) {
        RequestID[i] = 0;
    }

    /* Debug purpose only!: make believe for now that this comes from the customer */
    LogEntry logEntry1 {1, 10, 11, 12};
    serverState.smr_log.push_back(logEntry1);
    /* ------------------------------------------------------------------------*/

    if (nodeInfo.role == FOLLOWER){
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    timer.Start();
    while(true) {
        if (nodeInfo.role == LEADER) {
            Leader_Role (&serverState, &nodeInfo, &serverStub, poll_timeout, &PeerServerInfo,
                         &PeerIdIndexMap, Is_Init, Socket_Status, Socket, RequestID);
        }

        else if (nodeInfo.role == CANDIDATE) {
            Candidate_Role(&serverState, &nodeInfo, &serverStub, &timer, &PeerServerInfo,
                           &PeerIdIndexMap, Is_Init, Socket_Status, Socket);
        }

        else if (nodeInfo.role == FOLLOWER) {
            Follower_Role(&serverStub, &serverState, &timer, &nodeInfo);
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
                 bool *Socket_Status, int *Socket, int *RequestID){

    /* to-do: merge this with client interface. Wait for the request to arrive in the request queue
     * for some duration. If no request, break wait conditional variable and set heartbeat = true*/
    std::this_thread::sleep_for(std::chrono::milliseconds(poll_timeout / 2));

    bool heartbeat = true;
    Try_Connect(nodeInfo, serverStub, PeerServerInfo, Socket, Is_Init, Socket_Status);

    BroadCast_AppendEntryRequest(serverState, nodeInfo, serverStub, Socket,
                                 Is_Init, Socket_Status, RequestID, heartbeat);

    Get_Ack(serverState, poll_timeout, serverStub, PeerIdIndexMap, RequestID);
 }



void Candidate_Role(ServerState *serverState, NodeInfo *nodeInfo, ServerStub *serverStub,
                    ServerTimer *timer, std::vector<Peer_Info> *PeerServerInfo,
                    std::map<int,int> *PeerIdIndexMap, bool *Is_Init,
                    bool *Socket_Status, int *Socket) {

    int poll_timeout = timer -> Poll_timeout();
    bool Request_Completed [ nodeInfo -> num_peers ];

    Setup_New_Election(serverState, timer, nodeInfo, Request_Completed);

    /* While (not time out and vote has not been rejected) */
    while (!timer -> Check_election_timeout() && nodeInfo -> role == CANDIDATE) {

        Try_Connect(nodeInfo, serverStub, PeerServerInfo, Socket, Is_Init, Socket_Status);

        BroadCast_RequestVote(serverState, nodeInfo, serverStub, Socket, Is_Init,
                              Socket_Status, Request_Completed);

        Get_Vote(serverState, poll_timeout, nodeInfo, serverStub, Request_Completed,
                 PeerIdIndexMap);

        if ( nodeInfo -> role == LEADER) {
            Send_One_HeartBeat(serverState, nodeInfo, serverStub, timer, PeerServerInfo,
                               PeerIdIndexMap, Is_Init, Socket_Status, Socket);
        }
    }
}


void Follower_Role(ServerStub *serverStub, ServerState *serverState,
                   ServerTimer *timer, NodeInfo *nodeInfo){

    int poll_count;
    int poll_timeout = timer -> Poll_timeout();

    if ( timer -> Check_election_timeout() ) {
        nodeInfo -> role = CANDIDATE;
        std::cout << "Timeout: I'm the candidate now!" << '\n';
    }

    else {
        poll_count = serverStub -> Poll(poll_timeout * 2); // listen longer than Leader 2x
        if (poll_count > 0) {
            serverStub -> Handle_Poll_Follower(timer, serverState, nodeInfo);
        }
    }

}


