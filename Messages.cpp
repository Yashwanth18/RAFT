#include <cstring>
#include <arpa/inet.h>
#include <iostream>

#include "Messages.h"


/*---------------------------------------Leader Election----------------------------------*/
        /*-----------------VoteRequest Class----------------*/
VoteRequest::VoteRequest()  {
    term = -1;
    candidateId = -1;
    lastLogIndex = -1;
    lastLogTerm = -1;
}

void VoteRequest::
Set( int _term, int _candidateId, int _lastLogIndex, int _lastLogTerm){
    term = _term;
    candidateId = _candidateId;
    lastLogIndex = _lastLogIndex;
    lastLogTerm = _lastLogTerm;
}

void VoteRequest::Unmarshal(char *buffer){
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

void VoteRequest::Marshal(char *buffer){
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

int VoteRequest::Size() {
    int size = sizeof(term) + sizeof(candidateId) +
                sizeof(lastLogIndex)+ sizeof(lastLogTerm);
    return size;
}

int VoteRequest::Get_term() {
    return term;
}
int VoteRequest::Get_last_log_index() {
    return lastLogIndex;
}
int VoteRequest::Get_last_log_term() {
    return lastLogTerm;
}
int VoteRequest::Get_candidateId() {
    return candidateId;
}

void VoteRequest::Print(){
    std::cout << "\nterm: " << term << '\n';
    std::cout << "candidateId: " << candidateId << '\n';
    std::cout << "lastLogIndex: "<< lastLogIndex << '\n';
    std::cout << "lastLogTerm: "<< lastLogTerm << '\n';
    std::cout<<"--------------------------" <<'\n';
}


/* ----------------------------------ResponseVote----------------------------------- */
ResponseVote::ResponseVote()  {
    term = -1;
    voteGranted = false;
}

void ResponseVote::Set( int _term, int _voteGranted){
    term = _term;
    voteGranted = _voteGranted;
}

void ResponseVote::Unmarshal(char *buffer){

    int net_term;
    int net_vote_granted;


    int offset = 0; // first 4 bytes are for  field

    memcpy(&net_term, buffer + offset, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(&net_vote_granted, buffer + offset, sizeof(net_vote_granted));

    term = ntohl(net_term);
    voteGranted = ntohl(net_vote_granted);
}

void ResponseVote::Marshal(char *buffer){
    int net_term = htonl(term);
    int net_vote_granted = htonl(voteGranted);
    int offset = 0;

    memcpy(buffer + offset, &net_term, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(buffer + offset, &net_vote_granted, sizeof(net_vote_granted));


}

int ResponseVote::Size() {
    return sizeof(term) + sizeof(voteGranted);
}

void ResponseVote::Print() {
    std::cout<<"\nterm: "<< term <<'\n';
    std::cout<<"voteGranted: "<< voteGranted<<'\n';
    std::cout<<"--------------------------" <<'\n';
}

int ResponseVote::Get_voteGranted() {
    return voteGranted;
}
int ResponseVote::Get_term() {
    return term;
}

/*--------------------------------Log replication----------------------------------*/
    /*----------AppendEntryRequest class---------*/
AppendEntryRequest::AppendEntryRequest()  {
    sender_term = -1;
    leaderId = -1;
    prevLogTerm = -1;
    prevLogIndex = -1;
    logEntry.logTerm = -1;
    logEntry.opcode = -1;
    logEntry.arg1 = -1;
    logEntry.arg2 = -1;
    leaderCommit = -1;
    logRep_ID = -1;
}


void AppendEntryRequest:: Set( int _sender_term, int _leaderId,
                              int _prevLogTerm, int _prevLogIndex,
                              LogEntry * _logEntry, int _leaderCommit, int _logRep_ID){

    sender_term = _sender_term;
    leaderId = _leaderId;
    prevLogTerm = _prevLogTerm;
    prevLogIndex = _prevLogIndex;
    logEntry = *_logEntry;
    leaderCommit = _leaderCommit;
    logRep_ID = _logRep_ID;
}


void AppendEntryRequest::Unmarshal(char *buffer){
    int net_sender_term;
    int net_leaderId;
    int net_prevLogTerm;
    int net_prevLogIndex;
    int net_logTerm;
    int net_opcode;
    int net_arg1;
    int net_arg2;
    int net_leaderCommit;
    int net_logRep_ID;

    int offset = 0;

    memcpy(&net_sender_term, buffer + offset, sizeof(net_sender_term));
    offset += sizeof(net_sender_term);

    memcpy(&net_leaderId, buffer + offset, sizeof(net_leaderId));
    offset += sizeof(net_leaderId);

    memcpy(&net_prevLogTerm, buffer + offset, sizeof(net_prevLogTerm));
    offset += sizeof(net_prevLogTerm);

    memcpy(&net_prevLogIndex, buffer + offset, sizeof(net_prevLogIndex));
    offset += sizeof(net_prevLogIndex);

    memcpy(&net_logTerm, buffer + offset, sizeof(net_logTerm));
    offset += sizeof(net_logTerm);

    memcpy(&net_opcode, buffer + offset, sizeof(net_opcode));
    offset += sizeof(net_opcode);

    memcpy(&net_arg1, buffer + offset, sizeof(net_arg1));
    offset += sizeof(net_arg1);

    memcpy(&net_arg2, buffer + offset, sizeof(net_arg2));
    offset += sizeof(net_arg2);

    memcpy(&net_leaderCommit, buffer + offset, sizeof(net_leaderCommit));
    offset += sizeof(net_leaderCommit);

    memcpy(&net_logRep_ID, buffer + offset, sizeof(net_logRep_ID));

    sender_term = ntohl(net_sender_term);
    leaderId = ntohl(net_leaderId);
    prevLogTerm = ntohl(net_prevLogTerm);
    prevLogIndex = ntohl(net_prevLogIndex);
    logEntry.logTerm = ntohl(net_logTerm);
    logEntry.opcode = ntohl(net_opcode);
    logEntry.arg1 = ntohl(net_arg1);
    logEntry.arg2 = ntohl(net_arg2);
    leaderCommit = ntohl(net_leaderCommit);
    logRep_ID = ntohl(net_logRep_ID);
}

void AppendEntryRequest::Marshal(char *buffer){
    int net_sender_term = htonl(sender_term);
    int net_leaderId = htonl(leaderId);
    int net_prevLogTerm = htonl(prevLogTerm);
    int net_prevLogIndex = htonl(prevLogIndex);

    int net_logTerm = htonl(logEntry. logTerm);

    int net_opcode = htonl(logEntry. opcode);
    int net_arg1 = htonl(logEntry.arg1);

    int net_arg2 = htonl(logEntry.arg2);
    int net_leaderCommit = htonl(leaderCommit);
    int net_logRep_ID = htonl(logRep_ID);

    int offset = 0;

    memcpy(buffer + offset, &net_sender_term, sizeof(net_sender_term));
    offset += sizeof(net_sender_term);

    memcpy(buffer + offset, &net_leaderId, sizeof(net_leaderId));
    offset += sizeof(net_leaderId);

    memcpy(buffer + offset, &net_prevLogTerm, sizeof(net_prevLogTerm));
    offset += sizeof(net_prevLogTerm);

    memcpy(buffer + offset, &net_prevLogIndex, sizeof(net_prevLogIndex));
    offset += sizeof(net_prevLogIndex);

    memcpy(buffer + offset, &net_logTerm, sizeof(net_logTerm));
    offset += sizeof(net_logTerm);

    memcpy(buffer + offset, &net_opcode, sizeof(net_opcode));
    offset += sizeof(net_opcode);

    memcpy(buffer + offset, &net_arg1, sizeof(net_arg1));
    offset += sizeof(net_arg1);

    memcpy(buffer + offset, &net_arg2, sizeof(net_arg2));
    offset += sizeof(net_arg2);

    memcpy(buffer + offset, &net_leaderCommit, sizeof(net_leaderCommit));
    offset += sizeof(net_leaderCommit);

    memcpy(buffer + offset, &net_logRep_ID, sizeof(net_logRep_ID));
}

int AppendEntryRequest::Size() {
    return  sizeof(sender_term) + sizeof(leaderId) +
            sizeof(prevLogTerm) + sizeof(prevLogIndex) +
           sizeof(logEntry )+ sizeof (leaderCommit) + sizeof (logRep_ID);
}

void AppendEntryRequest::Print(){

    if (logEntry.logTerm == -1){
        std::cout << "\nReceived Heartbeat message " << '\n';
        std::cout << "sender_term : " << sender_term << '\n';
        std::cout << "leaderId : " << leaderId << '\n';
    }
    else{
        std::cout << "\nsender_term : " << sender_term << '\n';
        std::cout << "leaderId : " << leaderId << '\n';
        std::cout << "prevLogTerm : " << prevLogTerm << '\n';
        std::cout << "prevLogIndex : " << prevLogIndex << '\n';
        std::cout << "logTerm : "<< logEntry.logTerm << '\n';
        std::cout << "opcode : "<< logEntry.opcode << '\n';
        std::cout << "arg1 : " << logEntry.arg1 << '\n';
        std::cout << "arg2 : " << logEntry.arg2 << '\n';
        std::cout << "leaderCommit : " << leaderCommit << '\n';
        std::cout << "logRep_ID : " << logRep_ID << '\n';
        std::cout << "" << '\n';
    }
}


/* ----------------------Get private variables --------------------*/

int AppendEntryRequest::Get_sender_term() {
    return sender_term;
}
int AppendEntryRequest::Get_leaderId() {
    return leaderId;
}
int AppendEntryRequest::Get_prevLogTerm(){
    return prevLogTerm;
}
int AppendEntryRequest:: Get_prevLogIndex(){
    return prevLogIndex;
}


LogEntry AppendEntryRequest:: Get_LogEntry(){
    return logEntry;
}

int AppendEntryRequest:: Get_leaderCommit(){
    return leaderCommit;
}

int AppendEntryRequest::Get_logRep_ID() {
    return logRep_ID;
}


/*----------------------------ResponseAppendEntry class------------------*/
ResponseAppendEntry::ResponseAppendEntry()  {
    term = -1;
    success = -1;
    ResponseID = -1;
}


void ResponseAppendEntry::
Set( int _term, int _success, int _ResponseID){
    term = _term;
    success = _success;
    ResponseID = _ResponseID;
}


void ResponseAppendEntry::Unmarshal(char *buffer){
    int net_;
    int net_term;
    int net_success;
    int net_nodeID;
    int net_ResponseID;

    int offset = 0;

    memcpy(&net_, buffer + offset, sizeof(net_));
    offset += sizeof(net_);
    memcpy(&net_term, buffer + offset, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(&net_success, buffer + offset, sizeof(net_success));
    offset += sizeof(net_success);
    memcpy(&net_nodeID, buffer + offset, sizeof(net_nodeID));
    offset += sizeof(net_nodeID);
    memcpy(&net_ResponseID, buffer + offset, sizeof(net_ResponseID));

    term = ntohl(net_term);
    success = ntohl(net_success);
    ResponseID = ntohl(net_ResponseID);

}

void ResponseAppendEntry::Marshal(char *buffer){
    int net_term = htonl(term);
    int net_success = htonl(success);
    int net_ResponseID = htonl(ResponseID);
    int offset = 0;


    memcpy(buffer + offset, &net_term, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(buffer + offset, &net_success, sizeof(net_success));
    offset += sizeof(net_success);
    memcpy(buffer + offset, &net_ResponseID, sizeof(net_ResponseID));
}

int ResponseAppendEntry::Size() {
    return sizeof(term) + sizeof(success) + sizeof (ResponseID);
}

/* ----------------------Get private variables --------------------*/

int ResponseAppendEntry::Get_term(){
    return term;
}
int ResponseAppendEntry::Get_success(){
    return success;
}


int ResponseAppendEntry::Get_ResponseID() {
    return ResponseID;
}

void ResponseAppendEntry::Print(){

    std::cout << "\nterm : " << term << '\n';
    std::cout << "success : " << success << '\n';
    std::cout << "ResponseID : " << ResponseID << "" << '\n';
    std::cout << "" << '\n';
}
