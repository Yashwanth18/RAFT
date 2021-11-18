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
    int poll_count = 0;
    if (!Init_Node_Info (&node_info, argc, argv)){
      return 0;
    }

    clientstub.Init(&node_info, argc, argv);
   //   return 0;
    //}

    timer.Start();
    Poll_timeout = timer.Poll_timeout();

    while(true){
        poll_count = clientstub.Poll(Poll_timeout);
        if(poll_count != 0 )
        {
            clientstub.Handle_Follower_Poll(&timer,&node_info);
        }

    }

    return 1;
}
