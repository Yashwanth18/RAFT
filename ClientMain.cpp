#include "ClientMain.h"
#include "ClientTimer.h"
#include "ClientListenSocket.h"
#include <iomanip>


int main(int argc, char *argv[]) {
    ClientTimer timer;
    NodeInfo node_info;
    ClientStub clientstub;
    int Poll_timeout;
    int num_votes = 1;

    if (!Init_Node_Info (&node_info, argc, argv)){
      return 0;
    }

    if (!clientstub.Init(&node_info, argc, argv)) {
      return 0;
    }

    timer.Start();
    Poll_timeout = timer.Poll_timeout();

    while(true){
        clientstub.Poll(Poll_timeout);
        clientstub.Handle_Follower_Poll(&timer);
    }

    return 1;
}
