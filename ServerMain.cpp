#include "ServerMain.h"
#include "ServerTimer.h"
#include"ServerStub.h"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo node_info;
    ServerStub server_stub;
    VoteResponse voteResponse;
    int Poll_timeout;
    int role;
    int num_votes;

    /*Initialising to assume the role of the leader for debugging purpose*/
    role = CANDIDATE;

    if (!Init_Node_Info(&node_info, argc, argv)){
      return 0;
    }

    if (!server_stub.Init(&node_info, argc, argv)) {
      return 0;
    }

    timer.Start();
    Poll_timeout = timer.Poll_timeout();

    bool connected = false;
    num_votes = 0;

    while(true){
      if (role == CANDIDATE){

          if (!connected){
            server_stub.Connect_Follower();
            server_stub.Send_RequestVoteRPC(&node_info);
            connected = true;
          }

          int poll_count = server_stub.Poll(Poll_timeout);
          if (poll_count > 0) server_stub.Handle_Poll(&num_votes);


//           if (num_votes > node_info.num_peers / 2){
//              node_info.role = LEADER;
//           }
//          std::cout << "poll_count: " << poll_count << '\n';
//          node_info.role = LEADER;
      }

         if (role == LEADER){    //send heartbeat message
           //to-do: send real heartbeat message (empty log replication request)
             server_stub.Connect_Follower();
             server_stub.Send_AppendEntriesRPC(&node_info);
             connected = true;
         }

        // if (node_info.role == FOLLOWER){
        //     if (timer.Check_election_timeout()){
        //         node_info.role = CANDIDATE;
        //     }
        //     else{
        //         server_stub.Poll(Poll_timeout);
        //         server_stub.Handle_Follower_Poll(&timer);
        //     }
        // } //End follower role
    } //END while(true)

    return 1;
}

#pragma clang diagnostic pop