#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>
#include <vector>

#define FOLLOWER 0
#define CANDIDATE 1
#define LEADER 2

#define CLIENT_CONNECTION 1
#define SERVER_CONNECTION 2

#define VOTE_REQUEST 1
#define VOTE_RESPONSE 2
#define APPEND_ENTRY_REQUEST 3
#define APPEND_ENTRY_RESPONSE 4
#define CUSTOMER_REQUEST 5

struct Peer_Info{
  int unique_id;
  std::string IP;
  int port;

};

struct LogEntry{
    int logTerm;
    int opcode;
    int arg1;
    int arg2;
};

struct ServerState{
    /* Persistent state on all servers: Updated on stable storage before responding to RPCs */
    int currentTerm;
    int votedFor;
    std::vector<LogEntry> smr_log;

    /* volatile state on all servers */
    int commitIndex;
    int last_applied;

    /* volatile state on leaders (Reinitialized after election) */
    std::vector<int> nextIndex;     // size = num_peers
    std::vector<int> matchIndex;    // size = num_peers
};

struct NodeInfo{
    int node_id;
    int leader_id;
    int num_peers;

    int role;
    int server_port;
    int client_port;
};

/*-----------Log replication------------------*/
class AppendEntryRequest{
private:
    int messageType;

    int sender_term;     // term of the server that sent the request
    int leaderId;        // so follower can redirect clients

    /* prevLog info */
    int prevLogTerm;     // term of prevLogIndex entry
    int prevLogIndex;    // index of log entry immediately preceding new ones

    /* LogEntry */
    int logTerm;
    int opcode;
    int arg1;
    int arg2;
    /*-------------*/

    int leaderCommit;    // leader’s commitIndex

public:
    AppendEntryRequest();
    void Set(int _messageType, int _sender_term, int _leaderId,
             int _prevLogTerm, int _prevLogIndex,
             int _logTerm, int _opcode, int _arg1, int _arg2, int _leaderCommit);

    void Marshal(char *buffer);
    void UnMarshal(char * buffer);

    int Size();

    /* Get private variables */
    int Get_messageType();
    int Get_sender_term();
    int Get_leaderId();
    int Get_prevLogTerm();
    int  Get_prevLogIndex();
    int  Get_logTerm();
    int  Get_opcode();
    int  Get_arg1();
    int  Get_arg2();
    int  Get_leaderCommit();

    void Print();
};

class AppendEntryResponse{
private:
    int messageType;
    int term;     // currentTerm of the follower, for leader to update itself
    int success;  // true if follower contained entry matching prevLogIndex and prevLogTerm
    int nodeID;   // for the leader to keep track of which follower node has replicated the log
public:
    AppendEntryResponse();
    void Set(int _messageType, int _term, int _success, int _nodeID);

    void Marshal(char *buffer);
    void UnMarshal(char * buffer);

    int Size();

    /* Get private variables */
    int Get_messageType();
    int Get_term();
    int Get_success();
    int Get_nodeID();

    void Print();
};

/*-----------Customer Request------------------*/

class CustomerRequest{
private:
    int messageType;
    int logTerm;
    int opcode;
    int arg1;
    int arg2;
public:
    CustomerRequest();
    void Set(int _messageType, int _log_term, int _opcode, int _arg1, int _arg2);

    void Marshal(char *buffer);
    void UnMarshal(char * buffer);

    int Size();

    /* Get private variables */
    int Get_messageType();
    int Get_log_term();
    int Get_opcode();
    int Get_arg1();
    int Get_arg2();

    void Print();
};


/*---------------Response to customer-------------*/

class ResponseToCustomer{
private:
    int nodeRole;
    int leaderID;
public:
    ResponseToCustomer();
    void Set(int _node_role, int _leader_id);
    void Marshal(char *buffer);
    void UnMarshal(char *buffer);
    int Size();

    /* Get Private Variables*/
    int Get_node_role();
    int Get_leader_id();
};



#endif // #ifndef __MESSAGES_H__
