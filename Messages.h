#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>
struct VoteResult{
    int term;
    bool voteGranted;
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

    RequestVote();

};

#endif // #ifndef __MESSAGES_H__
