#include <poll.h>
#include <vector>
#include <string>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "ServerListenSocket.h"
#include "ServerTimer.h"

struct NodeInfo{
  int port;
  int node_id;
  int num_peers;

  int role;
  int leader_id;
};

class ServerStub{
private:
    std::vector<NodeInfo> PeerServerInfo;
    ServerListenSocket ListenSocket;
    int num_peers;
    int node_id;
    int port;
    int total_socket_num;

    //polling to avoid blocking. Initialisation.
    std::vector <pollfd> pfds;
    int alive_fd_count;

public:
    ServerStub() {};

    int Init(NodeInfo * node_info, int argc, char *argv[]);

    void Init_Listen_socket();
    int FillPeerServerInfo(int argc, char *argv[]);

    void Poll(int Poll_timeout);  //Poll_timeout is in millisecond
    void HandlePoll(ServerTimer * timer);

    void Accept_Connection();
    void Connect_To(std::string ip, int port);
    void Broadcast_nodeID();
    void Election_Protocol();
};
