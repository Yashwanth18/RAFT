#include "ServerMain.h"
#include "ServerTimer.h"
#include "ServerListenSocket.h"



int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo node_info;
    ServerStub stub;

    if (!Init_Node_Info (&node_info, argc, argv)) return 0;
    if (!stub.Init(&node_info, argc, argv)) return 0;

    timer.Start();
    while(true){

        if (node_info.role == LEADER){    //send heartbeat message
          stub.Broadcast_nodeID();
        }

        if (node_info.role == FOLLOWER){

            if (timer.Check_election_timeout()){
                node_info.role = CANDIDATE;
                stub.Election_Protocol();
                node_info.role = LEADER;
            }

            stub.Poll(timer.Poll_timeout());
            stub.HandlePoll(&timer);
        }

    } //END white(true)
}
