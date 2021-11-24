#include "ServerMain.h"

int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo nodeInfo;
    ServerStub serverStub;
    ServerTimer serverTimer;
    ServerState serverState;
    std::vector<Peer_Info> PeerServerInfo;
    std::map<int,int> PeerIdIndexMap;

    if (!Init_NodeInfo(&nodeInfo, argc, argv)){
        return 0;
    }

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo, &PeerIdIndexMap)){
        return 0;
    }

    Init_ServerState(&serverState, nodeInfo.num_peers);

    /* variables for error handling related to Socket*/
    int Socket[nodeInfo.num_peers];
    bool Socket_Status[nodeInfo.num_peers];  /* 0: Dead, 1: Alive */
    bool Is_Init[nodeInfo.num_peers];
    bool Request_Completed[nodeInfo.num_peers];

    serverStub.Init(&nodeInfo);
    Init_Socket(&serverStub, nodeInfo.num_peers, Socket, Is_Init, Socket_Status);

    /* Initialising to assume the role of the leader for debugging purpose*/
    nodeInfo.role = CANDIDATE;

    timer.Start();
    while(true){

        if (nodeInfo.role == CANDIDATE){
            Setup_New_Election(&serverState, &timer, &nodeInfo, Request_Completed);

            /* While (not time out and vote has not been rejected) */
            while (!timer.Check_election_timeout() && nodeInfo.role == CANDIDATE){

                Try_Connect(&nodeInfo, &serverStub, &PeerServerInfo,
                            Socket, Is_Init, Socket_Status, Request_Completed);

                BroadCast_Request_Vote(&serverState, &nodeInfo, &serverStub, Socket, Is_Init,
                                       Socket_Status, Request_Completed);

                Get_Vote(&serverState, &timer, &nodeInfo, &serverStub, Request_Completed,
                         &PeerIdIndexMap);

            } /* End: While (not time out and vote has not been rejected) */
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
         serverStub.Poll(Poll_timeout);
         serverStub.Handle_Follower_Poll(&timer);
     }
 }
*/




