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
#include <map>


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
    void Init(NodeInfo * nodeInfo);

    /* Accept Connection and Recv*/
    void Accept_Connection();
    void Add_Socket_To_Poll(int new_fd);
    int Poll(int poll_timeout);          /* Poll_timeout is in millisecond */

    /* Connect and Send */
    int Create_Socket();
    int Connect_To(std::string ip, int port, int fd);


    int SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo,
                               int fd, int peer_index);

    void FillAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo,
                         AppendEntryRequest *appendEntryRequest,  int peer_index);

    void Handle_Poll_Peer(ServerState *serverState, std::map<int,int> *PeerIdIndexMap,
                          bool* request_completed, int *num_ack, NodeInfo *nodeInfo);




};
