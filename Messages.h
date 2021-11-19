#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>

struct Peer_Info{
  int unique_id;
  std::string IP;
  int port;

};

struct NodeInfo{
    /* Persistent state on all servers: Updated on stable storage before responding to RPCs */
    int term;
    int votedFor;

    /* change this to a real vector of struct log */
    int lastLogTerm;
    int lastLogIndex;

    int node_id;
    int leader_id;

    int port;
    int num_peers;
};

/* -----------------Class for Request Vote -----------------*/
class RequestVote {
private:
    int term;
    int candidateId;
    int lastLogIndex;
    int lastLogTerm;

public:
    RequestVote();
    void Set(int term, int candidateId, int lastLogIndex, int lastLogTerm);

    void Marshal(char *buffer);
    void Unmarshal(char *buffer);

    /* get private variable function */
    int Get_term();
    int Get_candidateId();

    int Size();
    void Print();
};

/* -----------------Class for Response Vote -----------------*/
class VoteResponse{
private:
    int term;
    bool voteGranted;

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

#endif // #ifndef __MESSAGES_H__
