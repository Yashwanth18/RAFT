#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <iomanip>

#define FOLLOWER 0
#define CANDIDATE 1
#define LEADER 2

#include "Messages.h"

/* usage: ./client port nodeId
 * return 0 on failure and 1 on success
*/
int Init_Node_Info(NodeInfo * node_info, int argc, char *argv[]){
    if (argc < 3){
        std:: cout << "usage: ./client port nodeId" << '\n';
        return 0;
    }
    node_info -> leader_id = -1;
    node_info -> server_port = atoi(argv[1]);
    node_info -> node_id = atoi(argv[2]);


    return 1;
}

void Init_ServerState(ServerState * serverState){
    /* Persistent state on all servers: Updated on stable storage before responding to RPCs */
    serverState -> currentTerm = 0;
    serverState -> votedFor = -1;
    // std::vector<LogEntry> smr_log;


    /* volatile state on all servers */
    serverState -> commitIndex = 0;
    serverState -> last_applied = 0;
}