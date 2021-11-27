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
    int Send_ResponseAppendEntry(ResponseAppendEntry *ResponseAppendEntry, int fd);

    bool Set_Result(ServerState *serverState, AppendEntryRequest *appendEntryRequest);
    void Set_CommitIndex(AppendEntryRequest *appendEntryRequest, ServerState * serverState);

    void Print_Log(ServerState * serverState);
};
