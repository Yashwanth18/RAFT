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
#include "Messages.h"


class ClientStub{
private:
    ClientListenSocket ListenSocket;
    std::vector<pollfd> pfds;
public:
    ClientStub() {};

    void Init(int port);
    void Add_Socket_To_Poll(int new_fd);
    void Accept_Connection();
    int Poll(int Poll_timeout);  /* Poll_timeout is in millisecond; */

    void Handle_Follower_Poll(ServerState * serverState, ClientTimer * timer, NodeInfo *nodeInfo);
    //int Send_AppendEntryResponse(AppendEntryResponse *appendEntryResponse, int fd);
    int Send_Response_To_Customer(ResponseToCustomer *responseToCustomer, int fd);
//    bool Decide_Vote(NodeInfo *nodeInfo, RequestVote *requestVote);
//    bool Compare_Log(NodeInfo * nodeInfo, RequestVote * requestVote); /* to-do: to be implemented */

};
