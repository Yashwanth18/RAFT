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


class ServerCandidateStub: public ServerSocket{

public:
    ServerCandidateStub() {};

    /* ---------------Candidate helper functions ----------*/
    /**
     * Listens to the incoming vote response from the followers or append entry request from a
     * leader and takes necessary action i.e., if it's a vote response - it determines if the
     * the vote says that leader lags behind, then it changes the leader state to the follower, if
     * not it increments the vote count that it received, if the incoming request is of
     * append entry type, it would make itself as a follower
     * @param _pfds_server a vector of pollfd from where it reads the data ,i.e., either an
     *                     append entry request or a vote response
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param PeerIdIndexMap a map where key is the peer server id and value is its index in the
     *                       list of peers
     * @param request_completed a boolean array which represents the request for a specific
     *                          peer server
     * @param nodeInfo the struct containing the info about a server node
     */
    void Stub_Handle_Poll_Candidate(std::vector<pollfd> *_pfds_server, ServerState * serverState,
                               std::map<int,int> *PeerIdIndexMap, bool *request_completed,
                               NodeInfo *nodeInfo);

    /**
     * Candidate sends a request to all the followers sending details about its node information,
     * and about its log, and term.
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param nodeInfo the struct containing the info about a server node
     * @param fd the file descriptor to which the request for a vote is being sent
     * @return if the request is successfully sent, 0 if its a failure
     */
    int Stub_SendRequestVote(ServerState *serverState, NodeInfo *nodeInfo, int fd);

    /**
     * This function sets the request vote object that is to be sent to the followers
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param nodeInfo the struct containing the info about a server node
     * @param requestVote struct containing the request vote
     */
    void FillRequestVote(ServerState * serverState, NodeInfo *nodeInfo, RequestVote *requestVote);

};
