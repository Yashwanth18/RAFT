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
    ServerListenSocket ListenSocket;

    /* Design Modification: to-do ~ Use two ports given by the command line arguments.
     * One port listening for the peer servers and one port listening for the clients.
     */
    std::vector<pollfd> pfds_server;    /* for checking sockets connected to peer server. */

    /* Need a way to check if socket for talking to each peer server is already initialized */

    int num_peers;
    int port;
public:
    ServerStub() {};
    void Init(NodeInfo * node_info);

    /* Accept Connection */
    void Accept_Connection();
    void Add_Socket_To_Poll(int new_fd);

    /* Connect and Send */
    int Connect_To(std::string ip, int port);
    int SendRequestVote(NodeInfo *nodeInfo, int fd);

    void FillRequestVote(NodeInfo *nodeInfo, RequestVote *requestVote);

    /* Receive */
    int Poll(int poll_timeout);          /* Poll_timeout is in millisecond */
    void Handle_Poll_Peer(int *num_votes);
};
