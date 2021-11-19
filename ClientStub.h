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

    int Poll(int Poll_timeout);  //Poll_timeout is in millisecond;
    void Handle_Follower_Poll(ClientTimer * timer);

    void Accept_Connection();
};