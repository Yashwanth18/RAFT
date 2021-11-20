#include "ServerMain.h"
#include "ServerTimer.h"
#include "ServerStub.h"

int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo nodeInfo;
    ServerStub server_stub;
    ServerTimer serverTimer;
    std::vector<Peer_Info> PeerServerInfo;
    std::map<int,int> PeerIdIndexMap;

    int Poll_timeout;
    int poll_count;
    int num_votes;

    if (!Init_NodeInfo(&nodeInfo, argc, argv)){
        return 0;
    }

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo, &PeerIdIndexMap)){
        return 0;
    }

    server_stub.Init(&nodeInfo);
    Poll_timeout = timer.Poll_timeout();
    num_votes = 0;

    /* Initialising to assume the role of the leader for debugging purpose*/
    nodeInfo.role = CANDIDATE;

    int socket[nodeInfo.num_peers];
    bool socket_status[nodeInfo.num_peers];  /* 0: Dead, 1: Alive */
    bool is_init[nodeInfo.num_peers];
    bool request_completed[nodeInfo.num_peers];

    for (int i = 0; i < nodeInfo.num_peers; i++) {   /* Init Socket */
        socket[i] = server_stub.Create_Socket();
        is_init[i] = false;
        socket_status[i] = false;
    }

    int attempt = 0;
    timer.Start();

    while(true){

        if (nodeInfo.role == CANDIDATE){
            timer.Restart(); /* (re)start a new election */
            num_votes = 0;
            nodeInfo.term ++;

            for (int i = 0; i < nodeInfo.num_peers; i++){
                request_completed[i] = false;
            }


            int connect_status;

            /* While (not time out and vote has not been rejected) */
            while (!timer.Check_election_timeout() && nodeInfo.role == CANDIDATE){
                for (int i = 0; i < nodeInfo.num_peers; i++) {

                    if (!request_completed[i]) {
                        if (!is_init[i]) {
                            connect_status = server_stub.Connect_To(PeerServerInfo[i].IP,
                                                                    PeerServerInfo[i].port, socket[i]);
                            //                        std::cout << "attempt: " << attempt;
                            //                        std::cout << ", connect_status: " << connect_status << '\n';
                            attempt++;
                            if (connect_status) {
                                is_init[i] = true;
                                socket_status[i] = true;
                                server_stub.Add_Socket_To_Poll(socket[i]);
                            } else {
                                close(socket_status[i]);
                                socket[i] = server_stub.Create_Socket();
                            }
                        }
                    }
                }

                for (int i=0; i < nodeInfo.num_peers; i++) {

                    if (!request_completed[i]) {
                        if (socket_status[i]) {

                            std::cout << "sending request vote" << '\n';
                            if (!server_stub.SendRequestVote(&nodeInfo, socket[i])) { // if send fail

                                is_init[i] = false;
                                socket_status[i] = false;
                                close(socket[i]);
                                std::cout << "socket status is : " << socket_status[i] << '\n';
                                socket[i] = server_stub.Create_Socket(); // new socket

                            }
                        }
                    }
                }

                poll_count = server_stub.Poll(Poll_timeout);

                /* Do error handling for recv */
                if (poll_count > 0){
                    server_stub.Handle_Poll_Peer(&PeerIdIndexMap, request_completed, &num_votes, &nodeInfo);

                    if ( num_votes > nodeInfo.num_peers / 2 )
                    {
                        nodeInfo.role = LEADER;
                    }

                    std::cout << "half of the peers : " << nodeInfo.num_peers / 2 << '\n';
                    std::cout << "the role of the node is: " << nodeInfo.role << '\n';

                }


//                /* Send to all peers in parallel */
//                for (int i = 0; i < nodeInfo.num_peers; i++) {
//
//                    if (!request_completed[i]){     // request_completed?
//                        if (!is_init[i]) {

//                            //code moved up there

//                            close(socket[i]);
//                            socket[i] = server_stub.Create_Socket(); // new socket
//
//                            if (connect_status){
//                                is_init[i] = true;
//                                socket_status[i] = true;
//                                server_stub.Add_Socket_To_Poll(socket[i]);
//                            }
//                        }
//
//                        if (socket_status[i]){
//                            std::cout << "sending to file " << socket[i] << "..." << '\n';
//                            if (!server_stub.SendRequestVote(&nodeInfo, socket[i])){ // if send fail
//                                is_init[i] = false;
//                                socket_status[i] = false;
//
//                            }
//                        }
//
//
//                    } // End: request_completed?
//                } /* End: Send to all peers in parallel */
//
//                poll_count = server_stub.Poll(Poll_timeout);
//
//                /* Do error handling for recv */
//                if (poll_count > 0){
//                    server_stub.Handle_Poll_Peer( &num_votes, &nodeInfo);
//                    request_completed[0] = true;
//                    nodeInfo.role = LEADER;
//                }

            } // End: While (not time out and vote has not been rejected)
        } // End: Candidate role
    } // END: while(true)
}


// /*--------------------------Code below is for future implementation-----------------------*/
/*
 if (nodeInfo.role == LEADER){    //send heartbeat message
      to-do: check if there is a write request from the real client
      to-do: if no, send real heartbeat message (empty log replication request)
 }

 if (nodeInfo.role == FOLLOWER){
     if (timer.Check_election_timeout()){
         nodeInfo.role = CANDIDATE;
     }
     else{
         server_stub.Poll(Poll_timeout);
         server_stub.Handle_Follower_Poll(&timer);
     }
 }
*/




