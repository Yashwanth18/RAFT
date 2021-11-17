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

#include "ClientListenSocket.h"
#include "ClientTimer.h"

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

class ClientStub{
private:
    std::vector<Peer_Info> PeerClientInfo;
    ClientListenSocket ListenSocket;
    int num_peers;
    int node_id;
    int port;

    //polling to avoid blocking. Initialisation.
    std::vector<pollfd> pfds;
    int alive_connection;

public:
    ClientStub() {};

    //initialization
    int Init(NodeInfo * node_info, int argc, char *argv[]);
    int FillPeerClientInfo(int argc, char *argv[]);

    int Poll(int Poll_timeout);  //Poll_timeout is in millisecond;
    void Handle_Follower_Poll(ClientTimer * timer);

    void SendNodeID(int fd);
    void Accept_Connection();

    void Add_Socket_To_Poll(int new_fd);
    void Print_PeerClientInfo();
};
