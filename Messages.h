#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>
#define VoteGranted true
#define VoteDenied false
class VoteResponse{

public:

    int term;
    bool voteGranted;
    void Set_VoteResponse(int term, bool voteGranted);

    void Marshal(char *buffer);
    void Unmarshal(char *buffer);

    int Size();
    void Print();
    VoteResponse();
};
class RequestVote {
public:
    int term;
    int candidateId;
    int lastLogIndex;
    int lastLogTerm;

    void Set_RequestVote(int term, int candidateId, int lastLogIndex, int lastLogTerm);

    void Marshal(char *buffer);
    void Unmarshal(char *buffer);

    int Size();
    void Print();
    RequestVote();

};

#endif // #ifndef __MESSAGES_H__
