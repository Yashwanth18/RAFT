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


class ServerFollowerStub: public ServerSocket{

public:
    ServerFollowerStub() {};

    /* ---------------Follower helper function ----------*/

    /**
     * It handles both the append entry request if the request is from a leader, or a
     * request for a vote if the request is from a candidate. If its an append entry request,
     * it sets the result and sends it back to the leader, if its a request for a vote,
     * it sets if the vote can be granted or not and sends it back to the candidate
     * @param Timer the server timer to keep track if the follower becomes time out
     * @param _pfds_server a vector of pollfd from where it reads the data ,i.e., either an
     *                     append entry request or a request for a vote
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param nodeInfo the struct containing the info about a server node
     */
    void Stub_Handle_Poll_Follower(ServerTimer *Timer, std::vector<pollfd> *_pfds_server, ServerState *serverState,
                                   NodeInfo *nodeInfo);

    /**
     * This function handles append entry request coming from a leader, sets the append entry
     * response and sends it back to the leader
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param nodeInfo the struct containing the info about a server node
     * @param appendEntryRequest the struct containing the append entry request
     * @param buf buffer to store the unmarshalled the append entry object
     * @param fd the file descriptor of the leader, to which the follower sends the append entry
     *           response
     */
    void Handle_AppendEntryRequest(ServerState *serverState, NodeInfo *nodeInfo,
                                   AppendEntryRequest *appendEntryRequest, char *buf, int fd);

    /**
     * This function handles request vote coming from a candidate, sets the voting result, and
     * sends it back to the candidate
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param nodeInfo the struct containing the info about a server node
     * @param requestVote the struct containing the details about a vote request
     * @param buf buffer to store the unmarshalled the request vote object
     * @param fd the file descriptor of the candidate, to which the follower sends the vote
     *           response
     */
    void Handle_VoteRequest(ServerState *serverState, NodeInfo *nodeInfo,
                            RequestVote *requestVote, char *buf, int fd);

    /**
     * It unmarshalls the message type which helps in determining if the request is of type
     * vote request or append entry
     * @param buf to store the unmarshalled content of message type
     * @return the message type
     */
    int Unmarshal_MessageType(char *buf);

    /* For Log Replication Module */

    /**
     * It sends the response to the append entry request coming from the leader
     * @param ResponseAppendEntry struct containing response to the append entry request
     * @param fd the file descriptor of the leader to which response is being sent
     * @return 1 if the response is sent successfully, else 0
     */
    int Send_ResponseAppendEntry(ResponseAppendEntry *ResponseAppendEntry, int fd);

    /**
     * It sets the result of the append entry request by comparing the log length and the term of
     * both the follower and the leader
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param appendEntryRequest the struct containing the append entry request
     * @return true or false based on comparison with the incoming leader request
     */
    bool Set_Result(ServerState *serverState, AppendEntryRequest *appendEntryRequest);

    /**
     * Sets the commit index based on local log size and leaders commit index
     * @param appendEntryRequest the struct containing the append entry request
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     */
    void Set_CommitIndex(AppendEntryRequest *appendEntryRequest, ServerState * serverState);

    /**
     * It prints the log entries of the follower
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     */
    void Print_Log(ServerState *serverState);

    /* For Election Module */

    /**
     * It sends the vote response to the candidate
     * @param voteResponse struct containing the response to the vote request
     * @param fd the file descriptor of the candidate to whom, the follower sends its voting
     *           decision
     * @return 1 if it successfully sends the response, 0 if not
     */
    int SendVoteResponse(VoteResponse *voteResponse, int fd);

    /**
     * A function to decide the voting response to the candidate
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param nodeInfo the struct containing the info about a server node
     * @param requestVote the struct containing the details about a vote request
     * @return 0 or 1 based on the comparison between the term and log of the candidate
     *         and the follower
     */
    int Decide_Vote(ServerState *serverState, NodeInfo *nodeInfo, RequestVote *requestVote);

    /**
     * Compares the log between the candidate and the follower
     * @param serverState struct consisting the details about the servers log,
     *                    the current term of the server
     * @param nodeInfo the struct containing the info about a server node
     * @param requestVote the struct containing the details about a vote request
     * @return 0 or 1 based on the comparison between the term and log of the candidate
     *         and the follower
     */
    int Compare_Log(ServerState *serverState, NodeInfo * nodeInfo,RequestVote * requestVote);

};
