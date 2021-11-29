#include "ClientMain.h"
#include "ClientTimer.h"
#include "ClientListenSocket.h"
#include "ClientStub.h"
#include <chrono>
#include <thread>
/*
argv[1] is the port
*/
int main(int argc, char *argv[]) {
    ClientTimer timer;
    NodeInfo node_info;
    ServerState serverState;
    ClientStub clientstub;

    int Poll_timeout;


    if (!Init_Node_Info(&node_info, argc, argv)){
        return 0;
    }
    Init_ServerState(&serverState);

    clientstub.Init(atoi(argv[1])); // initialize the non-blocking listening port


    Poll_timeout = timer.Poll_timeout();

    /* Debug purpose only! */
    LogEntry logEntry1 {1, 0, 0, 0};
    serverState.smr_log.push_back(logEntry1);

    LogEntry logEntry2 {2, 0, 0, 0};
    serverState.smr_log.push_back(logEntry2);


    /*---------------------*/

    timer.Start();
    while(true){
        clientstub.Poll(Poll_timeout);
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        clientstub.Handle_Follower_Poll(&serverState, &timer, &node_info);
    }


}
