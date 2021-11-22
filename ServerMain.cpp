#include "ServerMain.h"

int main(int argc, char *argv[]) {
    NodeInfo nodeInfo;
    ServerState serverState;
    ServerStub serverStub;
    ServerTimer timer;

    std::vector<Peer_Info> PeerServerInfo;
    std::map<int,int> PeerIdIndexMap;

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo, &PeerIdIndexMap)){
        return 0;
    }

    if (!Init_NodeInfo(&nodeInfo, argc, argv)){
        return 0;
    }

    Init_ServerState(&serverState, nodeInfo.num_peers);
    serverStub.Init(&nodeInfo);

    /* variables for error handling related to Socket*/
    int Socket[nodeInfo.num_peers];
    bool Socket_Status[nodeInfo.num_peers];  /* 0: Dead, 1: Alive */
    bool Is_Init[nodeInfo.num_peers];
    bool Request_Completed[nodeInfo.num_peers];

    Init_Socket(&serverStub, nodeInfo.num_peers, Socket, Is_Init, Socket_Status);

    /* Initialising to assume the role of the leader for debugging purpose */
    nodeInfo.role = LEADER;
    serverState.currentTerm++;

    /* Debug purpose only!: make believe for now that this comes from the customer */
    LogEntry logEntry1 {serverState.currentTerm, 1, 11, 111};
    serverState.nextIndex[0]++;
    LogEntry logEntry2 {serverState.currentTerm, 2, 22, 222};

    serverState.smr_log.push_back(logEntry1);
    serverState.smr_log.push_back(logEntry2);
    /* ------------------------------------------------------------------------*/

    Init_Request_Completed_Bool(Request_Completed, nodeInfo.num_peers);
    bool all_replicated = false;
    int num_ack = 0;

    if (nodeInfo.role == LEADER){

        while(!all_replicated){
            Try_Connect(&nodeInfo, &serverStub, &PeerServerInfo,
                        Socket, Is_Init, Socket_Status, Request_Completed);

            BroadCast_AppendEntryRequest(&serverState, &nodeInfo, &serverStub, Socket,
                                         Is_Init, Socket_Status, Request_Completed);

            Get_Acknowledgement(&serverState, &timer, &nodeInfo, &serverStub, &num_ack,
                                Request_Completed, &PeerIdIndexMap);

            all_replicated = Check_Request_All_Completed(Request_Completed, nodeInfo.num_peers);
        }
    } // End: Leader role


}







