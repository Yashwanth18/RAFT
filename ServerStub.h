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


class ServerStub{
private:
    std::vector<Peer_Info> PeerServerInfo;
    ServerListenSocket ListenSocket;
    int num_peers;
    int node_id;
    int port;

    //polling to avoid blocking. Initialisation.
    std::vector<pollfd> pfds;

public:
    ServerStub() {};

    int Init(NodeInfo * node_info, int argc, char *argv[]);
    void Add_Socket_To_Poll(int new_fd);

    void Connect_Follower();
    void Send_RequestVoteRPC(NodeInfo *node_info);

    int Connect_To(std::string ip, int port);
    int SendRequestVote(RequestVote *requestVote, int fd);

    int Poll(int poll_timeout);          //Poll_timeout is in millisecond
    void Handle_Poll(int *num_votes);
    void Accept_Connection();


    /* peer server info */
    int FillPeerServerInfo(int argc, char *argv[]);
    void Print_PeerServerInfo();


    /*--------------Leader node (log replication) --------------------*/
    void Send_AppendEntriesRPC(NodeInfo *nodeInfo);
    int Send_AppendEntries(AppendEntries * appendEntries, int fd);
};
