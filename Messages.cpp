#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include "Messages.h"

/*-------------------RequestVote Class------------------*/
RequestVote::RequestVote()  {
    int term = -1;
    int candidateId = -1;
    int lastLogIndex = -1;
    int lastLogTerm = -1;
}


void RequestVote:: 
Set_RequestVote(int input_term, int input_candidateId, int input_lastLogIndex, int input_lastLogTerm){

    term = input_term;
    candidateId = input_candidateId;
    lastLogIndex = input_lastLogIndex;
    lastLogTerm = input_lastLogTerm;
}


void RequestVote::Unmarshal(char *buffer){
    int net_term;
    int net_candidateId;
    int net_lastLogIndex;
    int net_lastLogTerm;

    int offset = 0;

    memcpy(&net_term, buffer + offset, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(&net_candidateId, buffer + offset, sizeof(net_candidateId));
    offset += sizeof(net_candidateId);
    memcpy(&net_lastLogIndex, buffer + offset, sizeof(net_lastLogIndex));
    offset += sizeof(net_lastLogIndex);
    memcpy(&net_lastLogTerm, buffer + offset, sizeof(net_lastLogTerm));



    term = ntohl(net_term);
    candidateId = ntohl(net_candidateId);
    lastLogIndex = ntohl(net_lastLogIndex);
    lastLogTerm = ntohl(net_lastLogTerm);
}

void RequestVote::Marshal(char *buffer){
    int net_term = htonl(term);
    int net_candidateId = htonl(candidateId);
    int net_lastLogIndex = htonl(lastLogIndex);
    int net_lastLogTerm = htonl(lastLogTerm);

    int offset = 0;

    memcpy(buffer + offset, &net_term, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(buffer + offset, &net_candidateId, sizeof(net_candidateId));
    offset += sizeof(net_candidateId);
    memcpy(buffer + offset, &net_lastLogIndex, sizeof(net_lastLogIndex));
    offset += sizeof(net_lastLogIndex);
    memcpy(buffer + offset, &net_lastLogTerm, sizeof(net_lastLogTerm));
}

int RequestVote::Size() {
    return sizeof(term) + sizeof(candidateId) + sizeof(lastLogIndex)+
           sizeof(lastLogTerm);
}