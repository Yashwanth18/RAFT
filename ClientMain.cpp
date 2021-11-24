#include "ClientMain.h"
#include "ClientTimer.h"
#include "ClientListenSocket.h"
#include "ClientStub.h"


/*
argv[1] is the port
*/
int main(int argc, char *argv[]) {
    ClientTimer timer;
    ClientStub clientstub;
    int Poll_timeout;
    NodeInfo nodeInfo;
    ServerState serverState;

    if (!Init_NodeInfo(&nodeInfo, argc, argv)){
        return 0;
    }

    Init_ServerState(&serverState, nodeInfo.num_peers);
    clientstub.Init(atoi(argv[1])); //initialize the non-blocking listening port

    timer.Start();
    Poll_timeout = timer.Poll_timeout();

    while(true){
        clientstub.Poll(Poll_timeout);
        clientstub.Handle_Follower_Poll(&serverState, &timer, &nodeInfo);
    }


}
