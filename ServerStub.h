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
#include "Messages.h"
#include "ServerSocket.h"
struct Peer_Info{
  int unique_id;
  std::string IP;
  int port;

};
struct MapOp {
    int opcode ;
    int arg1 ;
    int arg2 ;
};

struct Log{
    MapOp command;
    int term;
};
struct NodeInfo{
  int port;
  int node_id; // candidate id
  int num_peers;
  int term;
  int last_log_index;
  int last_log_term;
  std::vector<Log> log_vector;
  // we need to create a log
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
    std::vector<SocketInfo> socket_info_vector;
    int alive_connection;

public:
    ServerStub() {};

    //initialization
    int Init(NodeInfo * node_info, int argc, char *argv[]);
    int FillPeerServerInfo(int argc, char *argv[]);

    int Poll(int Poll_timeout);  //Poll_timeout is in millisecond
//    int CountVote();

    void Handle_Poll(ServerTimer * timer);

//    void Broadcast_nodeID();
    void SendRequestVoteRPC(int fd, RequestVote * requestVote);
    void Connect_and_Send_RequestVoteRPC(NodeInfo * nodeInfo);

//    void Accept_Connection();
    int Connect_To(std::string ip, int port);

    void Add_Socket_To_Poll(int new_fd);
//    void Print_PeerServerInfo();
};
