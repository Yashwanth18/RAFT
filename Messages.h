#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>
#define LEADER_ELECTION 1
#define APPEND_ENTRIES 2
struct Peer_Info{
  int unique_id;
  std::string IP;
  int port;

};

struct NodeInfo{
    /* Persistent state on all servers: Updated on stable storage before responding to RPCs */
    int term;
    int votedFor;
    int role;

    /* change this to a real vector of struct log */
    int lastLogTerm;
    int lastLogIndex;

    int node_id;
    int leader_id;

    int port;
    int num_peers;

    /* log structure */
    int opcode;
    int arg1;
    int arg2;
};

/* -----------------Class for Request Vote -----------------*/
class RequestVote {
private:
    int term;
    int candidateId;
    int lastLogIndex;
    int lastLogTerm;
    int messageType;
public:
    RequestVote();
    void Set(int term, int candidateId, int lastLogIndex, int lastLogTerm);

    void Marshal(char *buffer);
    void Unmarshal(char *buffer);

    /* get private variable function */
    int Get_term();
    int Get_candidateId();
    int Get_last_log_index();
    int Size();
    void Print();
};

/* -----------------Class for Response Vote -----------------*/
class VoteResponse{
private:
    int term;
    bool voteGranted;
    int messageType;
public:
    VoteResponse();
    void Set(int term, bool voteGranted);

    void Marshal(char *buffer);
    void Unmarshal(char *buffer);

    int Size();
    void Print();

    /* get private variable function */
    bool Get_voteGranted();

};

/*-----------Log replication------------------*/
class AppendEntries{
private:
    int term;
    int node_id;
    int opcode;
    int arg1;
    int arg2;
    int messageType;
public:
    AppendEntries();
    void Set_AppendEntries(int node_id,int term, int opcode,int arg1, int arg2);
    void Marshal(char *buffer);
    void UnMarshal(char * buffer);
    int Get_term();
    int Get_id();
    int size();
};
#endif // #ifndef __MESSAGES_H__
