#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include "Messages.h"

/*------------------------------RequestVote Class-------------------------------*/
RequestVote::RequestVote()  {
     term = -1;
     candidateId = -1;
     lastLogIndex = -1;
     lastLogTerm = -1;
}


void RequestVote:: Set(int _term, int _candidateId, int _lastLogIndex, int _lastLogTerm){
    term = _term;
    candidateId = _candidateId;
    lastLogIndex = _lastLogIndex;
    lastLogTerm = _lastLogTerm;
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
    return sizeof(term) + sizeof(candidateId) + sizeof(lastLogIndex)+ sizeof(lastLogTerm);
}

int RequestVote::Get_term() {
    return term;
}
int RequestVote::Get_last_log_index() {
    return lastLogIndex;
}
int RequestVote::Get_candidateId() {
    return candidateId;
}

void RequestVote::Print(){
  std::cout << "term: " << term << '\n';
  std::cout << "candidateId: " << candidateId << '\n';
  std::cout << "lastLogIndex: "<< lastLogIndex << '\n';
  std::cout << "lastLogTerm: "<< lastLogTerm << '\n';
}


/* ----------------------------------voteResponse----------------------------------- */


VoteResponse::VoteResponse()  {
     term = -1;
     voteGranted = false;

}


void VoteResponse::Set(int _term, bool _voteGranted){

    term = _term;
    voteGranted = _voteGranted;
}


void VoteResponse::Unmarshal(char *buffer){
    int net_term;
    bool net_vote_granted;
    int offset = 0;

    memcpy(&net_term, buffer + offset, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(&net_vote_granted, buffer + offset, sizeof(net_vote_granted));

    term = ntohl(net_term);
    voteGranted = ntohl(net_vote_granted);

}

void VoteResponse::Marshal(char *buffer){
    int net_term = htonl(term);
    bool net_vote_granted = htonl(voteGranted);

    int offset = 0;

    memcpy(buffer + offset, &net_term, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(buffer + offset, &net_vote_granted, sizeof(net_vote_granted));
    //offset += sizeof(net_vote_granted);

}
void VoteResponse::Print() {
    std::cout<<"term: "<< term <<'\n';
    std::cout<<"voteGranted: "<< voteGranted<<'\n';
}

bool VoteResponse::Get_voteGranted() {
    return voteGranted;
}

int VoteResponse::Size() {
    return sizeof(term) + sizeof(voteGranted) ;
}


/*-------------------------------Append Entries class------------------*/

AppendEntries::AppendEntries()  {
    term = 0;
    opcode = -1;
    arg1 = -1;
    arg2 = -1;
}


void AppendEntries::Set_AppendEntries(int _term, int _opcode, int _arg1, int _arg2) {
    term = _term;
    opcode = _opcode;
    arg1 = _arg1;
    arg2 = _arg2;
}


void AppendEntries::UnMarshal(char *buffer){
    int net_term;
    int net_opcode;
    int net_arg1;
    int net_arg2;

    int offset = 0;

    memcpy(&net_term, buffer + offset, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(&net_opcode, buffer + offset, sizeof(net_opcode));
    offset += sizeof(net_opcode);
    memcpy(&net_arg1, buffer + offset, sizeof(net_arg1));
    offset += sizeof(net_arg1);
    memcpy(&net_arg2, buffer + offset, sizeof(net_arg2));

    term = ntohl(net_term);
    net_opcode = ntohl(net_opcode);
    net_arg1 = ntohl(net_arg1);
    net_arg2 = ntohl(net_arg2);
}

void AppendEntries::Marshal(char *buffer){
    int net_term = htonl(term);
    int net_opcode = htonl(opcode);
    int net_arg1 = htonl(arg1);
    int net_arg2 = htonl(arg2);

    int offset = 0;

    memcpy(buffer + offset, &net_term, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(buffer + offset, &net_opcode, sizeof(net_opcode));
    offset += sizeof(net_opcode);
    memcpy(buffer + offset, &net_arg1, sizeof(net_arg1));
    offset += sizeof(net_arg1);
    memcpy(buffer + offset, &net_arg2, sizeof(net_arg2));
}

int AppendEntries::size() {
    return sizeof(term) + sizeof(opcode) + sizeof(arg1)+ sizeof(arg2);
}