#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>
#include <vector>

#define FOLLOWER 0
#define CANDIDATE 1
#define LEADER 2

#define VOTE_REQUEST 1
#define RESPONSE_VOTE 2
#define APPEND_ENTRY_REQUEST 3
#define RESPONSE_APPEND_ENTRY 4

#define LEADER_ID_REQUEST 4


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

    /* volatile state on leaders (Reinitialized after Raft) */
    std::vector<int> nextIndex;     // size = num_peers
    std::vector<int> matchIndex;    // size = num_peers

    /* */
    int role;
    int num_votes;
    int leader_id;
};

struct NodeInfo{
    int node_id;
    int num_peers;
    int server_port;
    int client_port;
};
/*-----------------------------------Leader Raft----------------------------------*/

/* ----------------Request Vote Class-----------------*/
class VoteRequest {
private:
    int term;
    int candidateId;
    int lastLogIndex;
    int lastLogTerm;

public:
    VoteRequest();
    void Set(int _term, int _candidateId, int _lastLogIndex, int _lastLogTerm);

    void Marshal(char *buffer);
    void Unmarshal(char *buffer);

    /* get private variable function */
    int Get_term();
    int Get_candidateId();
    int Get_last_log_index();
    int Get_last_log_term();


    int Size();
    void Print();
};

/* -----------------Response Vote Class -----------------*/
class ResponseVote{
private:
    int term;
    int voteGranted;
public:
    ResponseVote();
    void Set(int _term, int _voteGranted);

    void Marshal(char *buffer);
    void Unmarshal(char *buffer);

    int Size();
    void Print();

    /* get private variable function */
    int Get_voteGranted();
    int Get_term();

};

/*-----------------------------------Log replication----------------------------------*/

/* -----AppendEntryRequest Class-----*/
class AppendEntryRequest{
private:
    int sender_term;     // term of the server that sent the request
    int leaderId;        // so follower can redirect clients
    int leaderCommit;    // leader’s commitIndex

    /* prevLog info */
    int prevLogTerm;     // term of prevLogIndex entry
    int prevLogIndex;    // index of log entry immediately preceding new ones

    LogEntry logEntry;

public:
    AppendEntryRequest();
    void Set(int _sender_term, int _leaderId, int _prevLogTerm, int _prevLogIndex,
             LogEntry * _logEntry, int _leaderCommit);

    void Marshal(char *buffer);
    void Unmarshal(char * buffer);

    int Size();

    /* Get private variables */
    int Get_sender_term();
    int Get_leaderId();
    int Get_prevLogTerm();
    int Get_prevLogIndex();
    LogEntry Get_LogEntry();
    int Get_leaderCommit();

    void Print();
};

/* -----ResponseAppendEntry Class-----*/
class ResponseAppendEntry{
private:
    int term;     // currentTerm of the follower, for leader to update itself
    int success;  // true if follower contained entry matching prevLogIndex and prevLogTerm
    int Heartbeat;
public:
    ResponseAppendEntry();
    void Set(int _term, int _success, int _Heartbeat);

    void Marshal(char *buffer);
    void Unmarshal(char * buffer);

    int Size();

    /* Get private variables */
    int Get_term();
    int Get_success();
    int Get_Heartbeat();

    void Print();
};


/*---------------------------Class: CustomerRequest---------------------------*/
class CustomerRequest {
private:
    int customer_id;
    int order_number;
    int request_type;

public:
    CustomerRequest();
    void operator = (const CustomerRequest &order) {
        customer_id = order.customer_id;
        order_number = order.order_number;
        request_type = order.request_type;
    }
    void SetOrder(int cid, int order_num, int type);
    int GetCustomerId();
    int GetOrderNumber();
    int GetRequestType();

    void Marshal(char *buffer);
    void Unmarshal(char *buffer);

    bool IsValid();
    int Size();
    void Print();
};

#endif // #ifndef __MESSAGES_H__