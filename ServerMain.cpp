#include "ServerMain.h"
#include "ServerTimer.h"
#include "ServerStub.h"


int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo node_info;
    ServerStub server_stub;
    ServerTimer serverTimer;
    std::vector<Peer_Info> PeerServerInfo;

    int Poll_timeout;
    int poll_count;
    int role;
    int num_votes;
    int new_fd;

    if (!Init_Node_Info(&node_info, argc, argv)){
        return 0;
    }

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo)){
        return 0;
    }

    server_stub.Init(&node_info);
    Poll_timeout = timer.Poll_timeout();
    num_votes = 0;

    /* Initialising to assume the role of the leader for debugging purpose*/
    role = CANDIDATE;

    bool request_completed = false;
    bool is_initialized = false;   // need to have this attached to each socket. use Vector? How to sync?
    /* Need to keep track of which peer has answered */

    timer.Start();
    while(true){
        /* need */
        if (role == CANDIDATE){
            timer.Restart(); /* (re)start a new election */
            num_votes = 0;

            while (!timer.Check_election_timeout()){
                if (!request_completed) {

                    /* Send to all peers in parallel */
                    for (int i = 0; i < node_info.num_peers; i++) {
                        if (!is_initialized) {
                            new_fd = server_stub.Connect_To(PeerServerInfo[i].IP, PeerServerInfo[i].port);
                        }

                        if (new_fd > 0) {        // we are connected
                            is_initialized = true;
                            server_stub.Add_Socket_To_Poll(new_fd);
                            server_stub.SendRequestVote(&node_info, new_fd);
                        }
                    } /* End: Send to all peers in parallel */

                    poll_count = server_stub.Poll(Poll_timeout);

                    if (poll_count > 0){
                        server_stub.Handle_Poll_Peer( &num_votes );
                        request_completed = true;
                    }
                }
            } // End: While (not time out)
        } // End: Candidate role
    } // END: while(true)
}


// /*--------------------------Code below is for future implementation-----------------------*/
/*
 if (node_info.role == LEADER){    //send heartbeat message
      to-do: check if there is a write request from the real client
      to-do: if no, send real heartbeat message (empty log replication request)
 }

 if (node_info.role == FOLLOWER){
     if (timer.Check_election_timeout()){
         node_info.role = CANDIDATE;
     }
     else{
         server_stub.Poll(Poll_timeout);
         server_stub.Handle_Follower_Poll(&timer);
     }
 }
*/




