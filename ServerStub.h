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
    std::vector<pollfd> pfds_server;    /* for checking sockets connected to peer server. */
public:
    ServerStub() {};
    void Init(NodeInfo * nodeInfo);

    /* Connect */
    int Create_Socket();
    int Connect_To(std::string ip, int port, int fd);
    int Poll(int poll_timeout);                         /* Poll_timeout is in millisecond */

    /* Accept Connection */
    void Accept_Connection();
    void Add_Socket_To_Poll(int new_fd);

    /* Candidate helper function */
    void Handle_Poll_Candidate(ServerState * serverState, std::map<int,int> *PeerIdIndexMap,
                          bool* request_completed, NodeInfo *nodeInfo);
    int SendRequestVote(ServerState *serverState, NodeInfo *nodeInfo, int fd);
    void FillRequestVote(ServerState * serverState, NodeInfo *nodeInfo, RequestVote *requestVote);

    /* follower helper functions */
    void Handle_Poll_Follower(std::vector<Peer_Info> * PeerServerInfo, ServerState *serverState, ServerTimer *timer, NodeInfo *nodeInfo);
    bool Decide_Vote(ServerState *serverState, NodeInfo *nodeInfo, RequestVote *requestVote);
    bool Compare_Log(ServerState *serverState, NodeInfo * nodeInfo,RequestVote * requestVote);
    int SendVoteResponse(VoteResponse *voteResponse, int fd);
    int SendResponseToCustomer(ResponseToCustomer *response_to_customer_from_follower, int fd);

    /* Leader helper functions */


};
