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
#include <map>

#include "ServerListenSocket.h"
#include "ServerTimer.h"
#include "Messages.h"
#include "ServerSocket.h"


class ServerLeaderStub: public ServerSocket{

public:
    ServerLeaderStub() {};

    /* ----------------------------------Leader helper function ------------------------------------*/

    /**
     * Leader looks for the append entry response, if the follower successfully acknowledges the
     * append entry, it increases the index to send the next log item, if not it decreases the
     * log index that needs to be sent, so that follower has the updated log info from the leader
     * @param _pfds_server a vector of pollfd from where it reads the data ,i.e., append entry
     *                     response
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param PeerIdIndexMap  a map where key is the peer server id and value is its index in the
     *                       list of peers
     * @param RequestID request id to determine if the request is of type log entry or a heartbeat
     *                  -1 if its a heartbeat
     */
    void Stub_Handle_Poll_Leader(std::vector<pollfd> *_pfds_server, ServerState *serverState,
                                 std::map<int,int> *PeerIdIndexMap, int * RequestID);

    /**
     * It sends append entry request to all the peers ,i.e., followers by sending its details about
     * the log, term
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param nodeInfo the struct containing the info about a server node
     * @param fd the file descriptor of the node to which the request is being sent
     * @param peer_index the peer_index of a follower
     * @param RequestID request id to determine if the request is of type log entry or a heartbeat
     *                  -1 if its a heartbeat
     * @return 1 if the request is successfully sent, 0 if its a failure
     */
    int Stub_SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo,
                               int fd, int peer_index, int RequestID);

    /**
     * It sets up the values in an append entry request that is to be sent to all the followers
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param nodeInfo the struct containing the info about a server node
     * @param appendEntryRequest the struct containing the append entry request
     * @param peer_index the peer_index of a follower
     * @param RequestID request id to determine if the request is of type log entry or a heartbeat
     *                  -1 if its a heartbeat
     */
    void FillAppendEntryRequest(ServerState * serverState, NodeInfo * nodeInfo,
                                AppendEntryRequest *appendEntryRequest,  int peer_index,
                                int RequestID);

};
