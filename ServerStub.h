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

struct Peer_Info{
  int unique_id;
  std::string IP;
  int port;

};

struct NodeInfo{
  int port;
  int node_id;
  int num_peers;

  int role;
  int leader_id;
};

//otherwise get the error "use of non-static data member" for initialising pfds
#define num_total_sockets 5

class ServerStub{
private:
    std::vector<Peer_Info> PeerServerInfo;
    ServerListenSocket ListenSocket;
    int num_peers;
    int node_id;
    int port;

    //polling to avoid blocking. Initialisation.
    std::vector<pollfd> pfds;
    int alive_connection;

public:
    ServerStub() {};

    //initialization
    int Init(NodeInfo * node_info, int argc, char *argv[]);
    int FillPeerServerInfo(int argc, char *argv[]);

    void Poll(int Poll_timeout);  //Poll_timeout is in millisecond
    int CountVote();

    void Handle_Follower_Poll(ServerTimer * timer);

    void Broadcast_nodeID();
    void SendNodeID(int fd);
    void Connect_and_Send_RequestVoteRPC();

    void Accept_Connection();
    int Connect_To(std::string ip, int port);

    void Add_Socket_To_Poll(int new_fd);
    void Print_PeerServerInfo();
};
