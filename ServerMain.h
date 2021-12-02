#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <iomanip>
#include <map>
#include <thread>
#include <fstream>
#include <sstream>

#include "ServerSocket.h"
#include "ServerOutStub.h"
#include "ServerThread.h"
#include "Messages.h"
#include "ServerTimer.h"

/* Usage: ./server port_server port_client nodeID num_peers (repeat PeerID IP port_server) */

/* return 0 on failure and 1 on success */
int Init_NodeInfo(NodeInfo * nodeInfo, int argc, char *argv[]){
    if (argc < 4){
        std::cout << "not enough arguments" << std::endl;
        return 0;
    }

    nodeInfo -> server_port = atoi(argv[1]);
    nodeInfo -> client_port = atoi(argv[2]);
    nodeInfo -> node_id = atoi(argv[3]);
    nodeInfo -> num_peers = atoi(argv[4]);
    return 1;
}


void Read_Logs_From_File(ServerState * serverState)
{

  std::string line;
  std::ifstream logFile ("example.txt");

  if (logFile.is_open()) {

    while (!logFile.eof()) {

      getline(logFile, line);

      std::stringstream temp(line);
      std::string intermediate_string;

      std::vector<int> tokens;
      LogEntry log_entry{};

      while (getline(temp, intermediate_string, ',')) {

        tokens.push_back(stoi(intermediate_string));
      }

      log_entry.logTerm = tokens.at(0);
      log_entry.opcode = tokens.at(1);
      log_entry.arg1 = tokens.at(2);
      log_entry.arg2 = tokens.at(3);

      serverState->smr_log.push_back(log_entry);
    }

    logFile.close();
  }
  else{
    std::cout << "cannot open file " << '\n';
    return;
  }
}

void Init_ServerState(ServerState * serverState, int num_peers, int argc, char *argv[]){
    /* Persistent state on all servers: Updated on stable storage before responding to RPCs */
    serverState -> currentTerm = 0;
    serverState -> votedFor = -1;
    LogEntry logEntry {-1, -1, -1, -1};
    serverState -> smr_log.push_back(logEntry);

    /* volatile state on leaders (Reinitialized after Raft) */
    for (int i = 0; i < num_peers; i++){
        serverState -> matchIndex.push_back(0);
        //serverState -> nextIndex.push_back(1);
    }

    /* volatile state on all servers */
    serverState -> commitIndex = 0;
    serverState -> last_applied = 0;


    serverState -> num_votes = 1;
    serverState -> leader_id = -1;
    serverState -> role = atoi(argv[argc - 1]);    /* for testing purpose only! */
}


/* return 0 on failure and 1 on success */
int FillPeerServerInfo(int argc, char *argv[], std::vector <Peer_Info> *PeerServerInfo){

    int num_peers = atoi(argv[4]);

    for (int i = 1; i <= num_peers; i++){

        if (argc <= 3*i + 4){
            std::cout << "not enough arguments" << std::endl;
            std::cout << "./server [port_server #] [port_client #] [unique ID] [# peers] "
                         "(repeat [ID] [IP] [port #])	" << std::endl;
            return 0;
        }

        else{
            int unique_id = atoi(argv[3*i + 2]);
            std::string IP = argv[3*i + 3];
            int server_port = atoi(argv[3*i + 4]);

            Peer_Info peer_server_info {unique_id, IP, server_port};
            PeerServerInfo -> push_back(peer_server_info);
        }

    } // END: for loop
    return 1;
}


void Candidate_Role(ServerState *serverState, NodeInfo *nodeInfo,
                    std::vector<Peer_Info> *PeerServerInfo,
                    std::vector <std::thread> *thread_vector, Raft *raft);