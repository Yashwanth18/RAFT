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
#include "ServerCandidateStub.h"
#include "ServerFollowerStub.h"
#include "ServerLeaderStub.h"

class ServerStub: public ServerSocket{
private:
    ServerListenSocket ListenSocket;
    std::vector<pollfd> pfds_server;    /* for checking sockets connected to peer server. */

    ServerFollowerStub serverFollowerStub;
    ServerCandidateStub serverCandidateStub;
    ServerLeaderStub serverLeaderStub;
public:

    ServerStub() {};

    /**
     * Initializes the listening socket to listen to the connections coming from both a peer
     * and a client
     * @param nodeInfo - the struct containing the info about a server node
     */
    void Init(NodeInfo * nodeInfo);

    /**
     * Returns the number of nodes which have readable data within the specified time limit
     * @param poll_timeout The timeout argument specifies the number of milliseconds that
       poll() should block waiting for a file descriptor to become ready
     * @return the number of nodes which has readable data
     */
    int Poll(int poll_timeout);          /* Poll_timeout is in millisecond */

    /**
     * It Adds the file descriptor of a server to the vector of pfds, with event as POLLIN
     * @param new_fd  the file descriptor of a server which is being polled for
     */
    void Add_Socket_To_Poll(int new_fd);

    /* ---------------Candidate helper functions ----------*/

    /**
     * Listens to the incoming vote response from the followers or append entry request from a
     * leader and takes necessary action i.e., if it's a vote response - it determines if the
     * the vote says that leader lags behind, then it changes the leader state to the follower, if
     * not it increments the vote count that it received, if the incoming request is of
     * append entry type, it would make itself as a follower
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param PeerIdIndexMap a map where key is the peer server id and value is its index in the
     *                       list of peers
     * @param request_completed a boolean array which represents the request for a specific
     *                          peer server
     * @param nodeInfo the struct containing the info about a server node
     */
    void Handle_Poll_Candidate(ServerState * serverState, std::map<int,int> *PeerIdIndexMap,
                               bool *request_completed, NodeInfo *nodeInfo);

    /**
     * Candidate sends a request to all the followers sending details about its node information,
     * and about its log, and term.
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param nodeInfo the struct containing the info about a server node
     * @param fd the file descriptor to which the request for a vote is being sent
     * @return 1 if the request is successfully sent, 0 if its a failure
     */
    int SendRequestVote(ServerState *serverState, NodeInfo *nodeInfo, int fd);


    /* ----------------------------------Leader helper function ------------------------------------*/

    /**
     * Leader looks for the append entry response, if the follower successfully acknowledges the
     * append entry, it increases the index to send the next log item, if not it decreases the
     * log index that needs to be sent, so that follower has the updated log info from the leader
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param PeerIdIndexMap a map where key is the peer server id and value is its index in the
     *                       list of peers
     * @param RequestID request id to determine if the request is of type log entry or a heartbeat
     *                  -1 if its a heartbeat
     */
    void Handle_Poll_Leader(ServerState *serverState, std::map<int,int> *PeerIdIndexMap, int * RequestID);

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
    int SendAppendEntryRequest(ServerState * serverState, NodeInfo *nodeInfo,
                               int fd, int peer_index, int RequestID);


    /* -------------------------------------Follower helper function ------------------------------------*/


    /**
     * It handles both the append entry request if the request is from a leader, or a
     * request for a vote if the request is from a candidate. If its an append entry request,
     * it sets the result and sends it back to the leader, if its a request for a vote,
     * it sets if the vote can be granted or not and sends it back to the candidate
     *
     * @param Timer the server timer to keep track if the follower becomes time out
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param nodeInfo the struct containing the info about a server node
     */
    void Handle_Poll_Follower(ServerTimer *Timer, ServerState *serverState, NodeInfo *nodeInfo);

};
