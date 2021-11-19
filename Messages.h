#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>

struct Peer_Info{
  int unique_id;
  std::string IP;
  int port;

};

struct NodeInfo{
    int port;
    int num_peers;

    /* Used in RequestVote*/
    int term;
    int node_id;
    int lastLogTerm;
    int lastLogIndex;

    int role;
    int leader_id;
};

class RequestVote {
private:
    int term;
    int candidateId;
    int lastLogIndex;
    int lastLogTerm;

public:
    RequestVote();
    void Set_RequestVote(int term, int candidateId,
                         int lastLogIndex, int lastLogTerm);

    void Marshal(char *buffer);
    void Unmarshal(char *buffer);

    int Size();
    void Print();
};

#endif // #ifndef __MESSAGES_H__
