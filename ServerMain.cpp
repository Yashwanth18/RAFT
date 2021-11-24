#include "ServerMain.h"

int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo nodeInfo;
    ServerStub serverStub;
    ServerState serverState;
    std::vector<Peer_Info> PeerServerInfo;
    std::map<int,int> PeerIdIndexMap;
    ServerTimer serverTimer;
    int poll_timeout = timer.Poll_timeout();

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


    timer.Start();
    while(true){

        if (nodeInfo.role == CANDIDATE){
            Setup_New_Election(&serverState, &timer, &nodeInfo, Request_Completed);

            /* While (not time out and vote has not been rejected) */
            while (!timer.Check_election_timeout() && nodeInfo.role == CANDIDATE){

                Try_Connect_Election(&nodeInfo, &serverStub, &PeerServerInfo,
                            Socket, Is_Init, Socket_Status, Request_Completed);

                BroadCast_RequestVote(&serverState, &nodeInfo, &serverStub, Socket, Is_Init,
                                       Socket_Status, Request_Completed);

                Get_Vote(&serverState, poll_timeout, &nodeInfo, &serverStub, Request_Completed,
                         &PeerIdIndexMap);

            } /* End: While (not time out and vote has not been rejected) */
        } // End: Candidate role

        // /*--------------------------Code below is for future implementation-----------------------*/

         else if (nodeInfo.role == LEADER){    //send heartbeat message
//              to-do: if no, send real heartbeat message (empty log replication request)
         }

         else if (nodeInfo.role == FOLLOWER){
             if (timer.Check_election_timeout()){
                 nodeInfo.role = CANDIDATE;
             }
             else{
                 serverStub.Poll(poll_timeout);
                 serverStub.Handle_Poll_Follower(&serverState, &timer, &nodeInfo);
             }
         }


    } // END: while(true)
}







