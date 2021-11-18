#include "ServerMain.h"



int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo node_info;
    ServerStub server_stub;
    int Poll_timeout;
    //int num_votes = 1;
    ServerSocket serverSocket;
    if (!Init_Node_Info (&node_info, argc, argv)){
      return 0;
    }

    if (!server_stub.Init(&node_info, argc, argv)) {
      return 0;
    }

    timer.Start();
    Poll_timeout = timer.Poll_timeout();
    while(true){

//        if (node_info.role == LEADER){    //send heartbeat message
//          //to-do: send real heartbeat message (empty log replication request)
//          server_stub.Broadcast_nodeID();
//        }
//
//        if (node_info.role == FOLLOWER){
//            if (timer.Check_election_timeout()){
//                node_info.role = CANDIDATE;
//            }
//            else{
//                server_stub.Poll(Poll_timeout); // what is the purpose of this?
//                server_stub.Handle_Follower_Poll(&timer); // what does it do?
//            }
//        } //End follower role

        if (node_info.role == CANDIDATE){

            server_stub.Connect_and_Send_RequestVoteRPC(&node_info); // have to implement actual RequestVoteRPC
            int poll_count = server_stub.Poll(Poll_timeout);
            if(poll_count > 0)
            {

            }
            //node_info.requestVote.Set_RequestVote(node_info.requestVote.term,node_info.node_id,node_info.requestVote.lastLogIndex,node_info.requestVote.lastLogTerm);
            //to count vote, need to keep track of which nodes has voted.
            //need to implement more structure.
            //num_votes += server_stub.CountVote();

            // if (num_votes > node_info.num_peers / 2){
            //    node_info.role = LEADER;
            // }
            //std::cout << "poll_count: " << poll_count << '\n';
            //node_info.role = LEADER;
        }
         //End candidate role

         // int poll_count = server_stub.Poll(Poll_timeout);
         // std::cout << "poll_count: " << poll_count << '\n';
    } //END white(true)
 // we are not ending the timer, is it fine?
    return 1;
}
