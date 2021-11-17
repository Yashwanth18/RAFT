#include "ServerMain.h"
#include "ServerTimer.h"
#include "ServerListenSocket.h"
#include <iomanip>


int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo node_info;
    ServerStub stub;
    int Poll_timeout;
    int num_votes = 1;

    if (!Init_Node_Info (&node_info, argc, argv)){
      return 0;
    }

    if (!stub.Init(&node_info, argc, argv)) {
      return 0;
    }

    timer.Start();
    Poll_timeout = timer.Poll_timeout();
    while(true){

        if (node_info.role == LEADER){    //send heartbeat message
          //to-do: send real heartbeat message (empty log replication request)
          stub.Broadcast_nodeID();
        }

        if (node_info.role == FOLLOWER){
            if (timer.Check_election_timeout()){
                node_info.role = CANDIDATE;
            }
            else{
                stub.Poll(Poll_timeout);
                stub.Handle_Follower_Poll(&timer);
            }
        } //End follower role

        if (node_info.role == CANDIDATE){
            stub.Connect_and_Send_RequestVoteRPC();
            stub.Poll(Poll_timeout);
            num_votes += stub.CountVote();

            // if (num_votes > node_info.num_peers / 2){
            //    node_info.role = LEADER;
            // }
            std::cout << "num_votes: " << num_votes << '\n';
            node_info.role = LEADER;
        } //End candidate role

    } //END white(true)

    return 1;
}
