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
    NodeInfo node_info;

    if (!Init_Node_Info(&node_info, argc, argv)){
        return 0;
    }

    clientstub.Init(atoi(argv[1])); //initialize the non-blocking listening port

    timer.Start();
    Poll_timeout = timer.Poll_timeout();

    while(true){
        clientstub.Poll(Poll_timeout);
        clientstub.Handle_Follower_Poll(&timer, &node_info);
    }


}
