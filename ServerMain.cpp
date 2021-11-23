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


    Init_Socket(&serverStub, nodeInfo.num_peers, Socket, Is_Init, Socket_Status);

    /* Initialising to assume the role of the leader for debugging purpose */
    nodeInfo.role = LEADER;

    /* Debug purpose only!: make believe for now that this comes from the customer */
    serverState.currentTerm ++;

    LogEntry logEntry1 {1, 0, 0, 0};
    serverState.smr_log.push_back(logEntry1);
    serverState.nextIndex[0]++;

    LogEntry logEntry2 {2, 0, 0, 0};
    serverState.smr_log.push_back(logEntry2);
    serverState.nextIndex[0]++;

    LogEntry logEntry3 {3, 0, 0, 0};
    serverState.smr_log.push_back(logEntry3);
    /* ------------------------------------------------------------------------*/


    int RequestID = 0;

    if (nodeInfo.role == LEADER){
//        int num_run = 100;
//        while (num_run > 0){
        while (true){


            Try_Connect(&serverState, &nodeInfo, &serverStub, &PeerServerInfo,
                        Socket, Is_Init, Socket_Status);

            BroadCast_AppendEntryRequest(&serverState, &nodeInfo, &serverStub,
                                         Socket, Is_Init, Socket_Status, &RequestID);

            Get_Ack(&serverState, &timer, &serverStub,
                    &PeerIdIndexMap, &RequestID);

//            std::cout << "num_run: " << num_run << '\n';
//            num_run --;
        }

    } // End: Leader role

}







