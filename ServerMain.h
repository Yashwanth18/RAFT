#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <iomanip>

#include "Messages.h"

#define FOLLOWER 0
#define CANDIDATE 1
#define LEADER 2


//return 0 on failure and 1 on success
int Init_Node_Info(NodeInfo * node_info, int argc, char *argv[]){
    if (argc < 4){
        std::cout << "not enough arguments" << std::endl;
        return 0;
    }

    node_info -> leader_id = -1;
    node_info -> node_id = atoi(argv[2]);

    node_info -> port = atoi(argv[1]);
    node_info -> num_peers = atoi(argv[3]);

    /* Used in RequestVote*/
    node_info -> term = 0;
    node_info ->  votedFor = -1;

     /* change this to a real vector of struct log */
    node_info ->  lastLogTerm = 0;
    node_info ->  lastLogIndex = 0;

  return 1;
}




