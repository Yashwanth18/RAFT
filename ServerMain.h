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

#include "ServerStub.h"
#include "ServerTimer.h"
#define FOLLOWER 0
#define CANDIDATE 1
#define LEADER 2


//return 0 on failure and 1 on success
int Init_Node_Info(NodeInfo * node_info, int argc, char *argv[]){
  if (argc < 4){
    std::cout << "not enough arguments" << std::endl;
    return 0;
  }

  //node_info -> role = FOLLOWER;
  /*Initialising to assume the role of the leader for debugging purpose*/
  node_info -> role = CANDIDATE;

  node_info -> port = atoi(argv[1]);
  node_info -> node_id = atoi(argv[2]);
  node_info -> num_peers = atoi(argv[3]);
  node_info -> leader_id = -1;
  node_info -> term = 0;
  node_info -> last_log_index = 0;
  node_info -> last_log_term = 0;
  return 1;
}
