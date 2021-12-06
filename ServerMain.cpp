#include "ServerMain.h"

int main(int argc, char *argv[]) {
    ServerTimer timer;
    NodeInfo nodeInfo;
    Raft raft; /* All server threads are here: Follower, Candidate, Leader */
    ServerSocket serverSocket;
    ServerSocket clientSocket;
    Interface interface;
    std::vector<Peer_Info> PeerServerInfo;
    MapClientRecord mapRecord;
    std::vector <std::thread> thread_vector;

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo))           { return 0; }
    if (!Init_NodeInfo(&nodeInfo, argc, argv))                      { return 0; }

    int _role = atoi(argv[argc - 1]); // for debugging and testing purposes
    ServerState serverState(nodeInfo.num_peers, _role);

    /* init a listening port to listen to peer servers */
    if (!serverSocket.Init(nodeInfo.server_port)) {
        std::cout << "Socket initialization failed" << '\n';
        return 0;
    }

    /* init a listening port to listen to customer */
    if (!clientSocket.Init(nodeInfo.client_port)) {
        std::cout << "Socket initialization failed" << '\n';
        return 0;
    }

    std::thread listen_clientThread(&Interface::Listening_Client, &interface,
                                    &clientSocket, &serverState,
                                    &thread_vector, &mapRecord);
    thread_vector.push_back(std::move(listen_clientThread));


    std::thread listenThread(&Raft::ListeningThread, &raft, &serverSocket, &serverState,
                             &thread_vector,  &timer, &mapRecord);
    thread_vector.push_back(std::move(listenThread));

    /* for testing purposes: allow the programmer time to run other nodes */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    while(true){
        timer.Atomic_Restart();

        if (serverState.role == FOLLOWER) {
            while(!timer.Check_Election_timeout()){}    /* do nothing */
            serverState.SetRole(CANDIDATE);    /* if election timeout */
        }

        else if (serverState.role == CANDIDATE) {
            Candidate_Role(&serverState, &nodeInfo, &PeerServerInfo,
                           &thread_vector, &raft);
        }

        else if (serverState.role == LEADER) {
            // to-do: upon election: send a heartbeat message. engineer the initialization

            for (int i = 0; i < nodeInfo.num_peers; i++){
                std::thread leader_thread(&Raft::LeaderThread, &raft, i, &PeerServerInfo,
                                          &nodeInfo, &serverState, &mapRecord, &timer);
                thread_vector.push_back(std::move(leader_thread));
            }

            /* do nothing :just so that we do not spin infinite number of threads */
            while (true) {}
        }

        else {
            std::cout << "Undefined Server Role Initialization" << '\n';
        }
    }

}  // End: Main

/* -------------------------------End: Main Function------------------------------------ */

/* ------------------Candidate Helper Functions  ------------------*/
void Candidate_Role(ServerState *serverState, NodeInfo *nodeInfo,
                    std::vector<Peer_Info> *PeerServerInfo,
                    std::vector <std::thread> *thread_vector,
                    Raft *raft){

    ServerTimer _timer;
    serverState -> NewElection(nodeInfo -> node_id, nodeInfo -> num_peers);
    int half_peers = nodeInfo -> num_peers / 2;
    int num_votes;

    for (int i = 0; i < nodeInfo -> num_peers; i++){
        std::thread candidate_thread(&Raft::CandidateThread, raft, i, PeerServerInfo,
                                     nodeInfo, serverState);
        thread_vector -> push_back(std::move(candidate_thread));
    }

    _timer.Start();

    while(!_timer.Check_Election_timeout()){
        num_votes = serverState -> Get_numVotes();

        if (num_votes > half_peers){
            serverState -> Become_Leader(nodeInfo -> node_id);
            break;
        }
    }

    if (serverState -> GetRole() != LEADER) {
        serverState -> SetRole(FOLLOWER);
    }
}