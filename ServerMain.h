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
#include "ServerConfiguration.h"
std::map<int, std::vector<std::string>> configMap;


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

void Init_ServerState(ServerState * serverState, NodeInfo * nodeInfo, int argc, char *argv[]){
    /* Persistent state on all servers: Updated on stable storage before responding to RPCs */


    if(!configMap.count(nodeInfo->node_id)){
      std::vector<std::string> fileVector ;
      std::string smr_log_file_name = "node_" + std::to_string(nodeInfo -> node_id) + "smr_log_file.txt";
      std::string server_state_file_name = "node_" + std::to_string(nodeInfo -> node_id) + "server_state.txt";
      fileVector.push_back(smr_log_file_name);
      fileVector.push_back(server_state_file_name);
      std::ofstream smr_log_file (smr_log_file_name);
      std::ofstream server_state_file (server_state_file_name);
      configMap [nodeInfo -> node_id] = fileVector;
      serverState -> currentTerm = 0;
      serverState -> votedFor = -1;
      LogEntry logEntry {-1, -1, -1, -1};
      serverState -> smr_log.push_back(logEntry);
    }

    else{ // to-do: clean this up
      std::vector<std::string> vector_file  = configMap.at(nodeInfo -> node_id);
      std::string log_smr_file = vector_file.front();
      std::string server_state_file = vector_file.back();
      std::ifstream read_smr_log_file (log_smr_file);
      std::ifstream read_server_state_file (server_state_file);
      std::string log_line;
      std::string server_state_line;
      if(read_smr_log_file.is_open()) {

        while (!read_smr_log_file.eof()) {
          getline(read_smr_log_file, log_line);

          std::stringstream temp(log_line);
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
      }

      if(read_server_state_file.is_open()){
        read_server_state_file >> server_state_line;
        std::string delimiter = ",";

        size_t pos = 0;
        std::string token;
        std::vector<std::string> server_state_values;
        while ((pos = server_state_line.find(delimiter)) != std::string::npos) {
          token = server_state_line.substr(0, pos);
          server_state_values.push_back(token);
          server_state_line.erase(0, pos + delimiter.length());
        }
        server_state_values.push_back(server_state_line);
        serverState -> currentTerm = std::stoi(server_state_values[0]);
        serverState -> votedFor = std::stoi(server_state_values[1]);
      }
    }
    serverState -> nodeId = nodeInfo -> node_id;

    /* volatile state on leaders (Reinitialized after Raft) */
    for (int i = 0; i < nodeInfo->num_peers; i++){
        serverState -> matchIndex.push_back(0);
        serverState -> nextIndex.push_back(1);
    }

    /* volatile state on all servers */
    serverState -> commitIndex = 0;
    serverState -> last_applied = 0;


    serverState -> num_votes = 1;            /* 1 because always vote for oneself */
    serverState -> leader_id = -1;
    serverState -> role = atoi(argv[argc - 1]);    /* for testing purpose only! */
}



void Write_ServerStateToAStorage(ServerState *serverState) {
  if(configMap.count(serverState -> nodeId)){
    std::vector<std::string> vector_file  = configMap.at(serverState -> nodeId);
    std::string server_state_file = vector_file.back();
    std::ofstream write_server_state (server_state_file);
    write_server_state << serverState -> currentTerm << "," << serverState -> votedFor;
    write_server_state.close();
  }
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

void SetRole_Atomic(ServerState *serverState, std::mutex *lk_serverState, int _role){
    lk_serverState -> lock();        // lock

    serverState -> role = _role;
    if (_role == LEADER){
        std::cout << "Becoming a leader now!" << '\n';
    }

    else if (_role == FOLLOWER){
        std::cout << "num_votes: " << serverState -> num_votes << '\n';
        std::cout << "Resigning to be a follower now!" << '\n';
    }
    else if (_role == CANDIDATE){
        std::cout << "Becoming a candidate now!" << '\n';
    }

    lk_serverState -> unlock();     // unlock
}

/* --------------------------Functions Declaration-------------------------*/

void Candidate_Role(ServerState *serverState, NodeInfo *nodeInfo,
                    std::vector<Peer_Info> *PeerServerInfo,
                    std::vector <std::thread> *thread_vector,
                    Raft *raft, std::mutex *lk_serverState);