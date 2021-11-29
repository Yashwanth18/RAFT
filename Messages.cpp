#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include "Messages.h"

void Print_MessageType(int messageType){
    if (messageType == VOTE_REQUEST){
        std::cout << "\nmessageType: VoteRequest" << '\n';
    }
    else if (messageType == RESPONSE_VOTE){
        std::cout << "\nmessageType: ResponseVote" << '\n';
    }
    else if (messageType == APPEND_ENTRY_REQUEST){
        std::cout << "\nmessageType: AppendEntryRequest" << '\n';
    }
    else if(messageType == RESPONSE_APPEND_ENTRY){
        std::cout << "\nmessageType: ResponseAppendEntry" << '\n';
    }
}
/*---------------------------------------Leader Election----------------------------------*/
        /*-----------------VoteRequest Class----------------*/
VoteRequest::VoteRequest()  {
    messageType = -1;
    term = -1;
    candidateId = -1;
    lastLogIndex = -1;
    lastLogTerm = -1;
}

void VoteRequest::
Set(int _messageType, int _term, int _candidateId, int _lastLogIndex, int _lastLogTerm){

    messageType = _messageType;
    term = _term;
    candidateId = _candidateId;
    lastLogIndex = _lastLogIndex;
    lastLogTerm = _lastLogTerm;
}

void VoteRequest::Unmarshal(char *buffer){
    int net_messageType;
    int net_term;
    int net_candidateId;
    int net_lastLogIndex;

    int net_lastLogTerm;

    int offset = 0;

    memcpy(&net_messageType, buffer + offset, sizeof(net_messageType));
    offset += sizeof(net_messageType);
    memcpy(&net_term, buffer + offset, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(&net_candidateId, buffer + offset, sizeof(net_candidateId));
    offset += sizeof(net_candidateId);
    memcpy(&net_lastLogIndex, buffer + offset, sizeof(net_lastLogIndex));
    offset += sizeof(net_lastLogIndex);
    memcpy(&net_lastLogTerm, buffer + offset, sizeof(net_lastLogTerm));

    messageType = ntohl(net_messageType);
    term = ntohl(net_term);
    candidateId = ntohl(net_candidateId);
    lastLogIndex = ntohl(net_lastLogIndex);
    lastLogTerm = ntohl(net_lastLogTerm);
}

void VoteRequest::Marshal(char *buffer){
    int net_messageType = htonl(messageType);
    int net_term = htonl(term);
    int net_candidateId = htonl(candidateId);
    int net_lastLogIndex = htonl(lastLogIndex);
    int net_lastLogTerm = htonl(lastLogTerm);

    int offset = 0;

    memcpy(buffer + offset, &net_messageType, sizeof(net_messageType));
    offset += sizeof(net_term);
    memcpy(buffer + offset, &net_term, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(buffer + offset, &net_candidateId, sizeof(net_candidateId));
    offset += sizeof(net_candidateId);
    memcpy(buffer + offset, &net_lastLogIndex, sizeof(net_lastLogIndex));
    offset += sizeof(net_lastLogIndex);
    memcpy(buffer + offset, &net_lastLogTerm, sizeof(net_lastLogTerm));
}

int VoteRequest::Size() {
    return sizeof (messageType) + sizeof(term) + sizeof(candidateId) +
           sizeof(lastLogIndex)+ sizeof(lastLogTerm);
}

/* ----------------------Get private variables --------------------*/
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
int VoteRequest::Get_message_type() {
    return messageType;
}

void VoteRequest::Print(){
    ::Print_MessageType(messageType);
    std::cout << "term: " << term << '\n';
    std::cout << "candidateId: " << candidateId << '\n';
    std::cout << "lastLogIndex: "<< lastLogIndex << '\n';
    std::cout << "lastLogTerm: "<< lastLogTerm << '\n';
    std::cout<<"--------------------------" <<'\n';
}


/* ----------------------------------ResponseVote----------------------------------- */
ResponseVote::ResponseVote()  {
    messageType = -1;
    term = -1;
    voteGranted = false;
    node_id = -1;
}

void ResponseVote::Set(int _messageType, int _term, int _voteGranted, int _node_id){
    messageType = _messageType;
    term = _term;
    voteGranted = _voteGranted;
    node_id = _node_id;

}

void ResponseVote::Unmarshal(char *buffer){
    int net_messageType;
    int net_term;
    int net_vote_granted;
    int net_node_id;

    int offset = 0; // first 4 bytes are for messageType field

    memcpy(&net_messageType, buffer + offset, sizeof(net_messageType));
    offset += sizeof(net_messageType);
    memcpy(&net_term, buffer + offset, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(&net_vote_granted, buffer + offset, sizeof(net_vote_granted));
    offset += sizeof(net_vote_granted);
    memcpy(&net_node_id, buffer + offset, sizeof(net_node_id));

    messageType = ntohl(net_messageType);
    term = ntohl(net_term);
    voteGranted = ntohl(net_vote_granted);
    node_id = ntohl(net_node_id);
}

void ResponseVote::Marshal(char *buffer){
    int net_messageType = htonl(messageType);
    int net_term = htonl(term);
    int net_vote_granted = htonl(voteGranted);
    int net_node_id = htonl(node_id);

    int offset = 0;

    memcpy(buffer + offset, &net_messageType, sizeof(net_messageType));
    offset += sizeof(net_messageType);
    memcpy(buffer + offset, &net_term, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(buffer + offset, &net_vote_granted, sizeof(net_vote_granted));
    offset += sizeof(net_vote_granted);
    memcpy(buffer + offset, &net_node_id, sizeof(net_node_id));

}

int ResponseVote::Size() {
    return sizeof(messageType) + sizeof(term) + sizeof(voteGranted) + sizeof (node_id);
}

void ResponseVote::Print() {
    ::Print_MessageType(messageType);
    std::cout<<"term: "<< term <<'\n';
    std::cout<<"voteGranted: "<< voteGranted<<'\n';
    std::cout<<"node_id: "<< node_id<<'\n';
    std::cout<<"--------------------------" <<'\n';
}

/* ----------------------Get private variables --------------------*/
int ResponseVote::Get_messageType() {
    return messageType;
}
int ResponseVote::Get_voteGranted() {
    return voteGranted;
}
int ResponseVote::Get_nodeID() {
    return node_id;
}
int ResponseVote::Get_term() {
    return term;
}

/*--------------------------------Log replication----------------------------------*/
    /*----------AppendEntryRequest class---------*/
AppendEntryRequest::AppendEntryRequest()  {
    messageType = -1;
    sender_term = -1;
    leaderId = -1;
    prevLogTerm = -1;
    prevLogIndex = -1;
    logEntry.logTerm = -1;
    logEntry.opcode = -1;
    logEntry.arg1 = -1;
    logEntry.arg2 = -1;
    leaderCommit = -1;
    RequestID = -1;
}


void AppendEntryRequest:: Set(int _messageType, int _sender_term, int _leaderId,
                              int _prevLogTerm, int _prevLogIndex,
                              LogEntry * _logEntry, int _leaderCommit, int _RequestID){

    messageType = _messageType;
    sender_term = _sender_term;
    leaderId = _leaderId;
    prevLogTerm = _prevLogTerm;
    prevLogIndex = _prevLogIndex;
    logEntry = *_logEntry;
    leaderCommit = _leaderCommit;
    RequestID = _RequestID;
}


void AppendEntryRequest::Unmarshal(char *buffer){
    int net_messageType;
    int net_sender_term;
    int net_leaderId;
    int net_prevLogTerm;
    int net_prevLogIndex;
    int net_logTerm;
    int net_opcode;
    int net_arg1;
    int net_arg2;
    int net_leaderCommit;
    int net_RequestID;
    
    int offset = 0;

    memcpy(&net_messageType, buffer + offset, sizeof(net_messageType));
    offset += sizeof(net_messageType);
    
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

    memcpy(&net_RequestID, buffer + offset, sizeof(net_RequestID));

    messageType = ntohl(net_messageType);
    sender_term = ntohl(net_sender_term);
    leaderId = ntohl(net_leaderId);
    prevLogTerm = ntohl(net_prevLogTerm);
    prevLogIndex = ntohl(net_prevLogIndex);
    logEntry.logTerm = ntohl(net_logTerm);
    logEntry.opcode = ntohl(net_opcode);
    logEntry.arg1 = ntohl(net_arg1);
    logEntry.arg2 = ntohl(net_arg2);
    leaderCommit = ntohl(net_leaderCommit);
    RequestID = ntohl(net_RequestID);
}

void AppendEntryRequest::Marshal(char *buffer){
    int net_messageType = htonl(messageType);
    
    int net_sender_term = htonl(sender_term);
    int net_leaderId = htonl(leaderId);
    int net_prevLogTerm = htonl(prevLogTerm);
    int net_prevLogIndex = htonl(prevLogIndex);
    
    int net_logTerm = htonl(logEntry. logTerm);

    int net_opcode = htonl(logEntry. opcode);
    int net_arg1 = htonl(logEntry.arg1);

    int net_arg2 = htonl(logEntry.arg2);
    int net_leaderCommit = htonl(leaderCommit);
    int net_RequestID = htonl(RequestID);

    int offset = 0;

    memcpy(buffer + offset, &net_messageType, sizeof(net_messageType));
    offset += sizeof(net_messageType);

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

    memcpy(buffer + offset, &net_RequestID, sizeof(net_RequestID));
}

int AppendEntryRequest::Size() {
    return sizeof(messageType) + sizeof(sender_term) + sizeof(leaderId) +
            sizeof(prevLogTerm) + sizeof(prevLogIndex) +
           sizeof(logEntry )+ sizeof (leaderCommit) + sizeof (RequestID);
}

void AppendEntryRequest::Print(){
    ::Print_MessageType(messageType);
    std::cout << "sender_term : " << sender_term << '\n';
    std::cout << "leaderId : " << leaderId << '\n';
    std::cout << "prevLogTerm : " << prevLogTerm << '\n';
    std::cout << "prevLogIndex : " << prevLogIndex << '\n';
    std::cout << "logTerm : "<< logEntry.logTerm << '\n';
    std::cout << "opcode : "<< logEntry.opcode << '\n';
    std::cout << "arg1 : " << logEntry.arg1 << '\n';
    std::cout << "arg2 : " << logEntry.arg2 << '\n';
    std::cout << "leaderCommit : " << leaderCommit << '\n';
    std::cout << "RequestID : " << RequestID << '\n';
    std::cout << "---------------------------" << '\n';
}


/* ----------------------Get private variables --------------------*/
int AppendEntryRequest::Get_messageType(){
    return messageType;
}
int AppendEntryRequest::Get_term(){
    return sender_term;
}
int AppendEntryRequest::Get_nodeID(){
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

int AppendEntryRequest::Get_RequestID() {
    return RequestID;
}


    /*------------ResponseAppendEntry class------------------*/
ResponseAppendEntry::ResponseAppendEntry()  {
    messageType = -1;
    term = -1;
    success = -1;
    nodeID = -1;
    ResponseID = -1;
}


void ResponseAppendEntry::
Set(int _messageType, int _term, int _success, int _nodeID, int _ResponseID){
    messageType = _messageType;
    term = _term;
    success = _success;
    nodeID = _nodeID;
    ResponseID = _ResponseID;
}


void ResponseAppendEntry::Unmarshal(char *buffer){
    int net_messageType;
    int net_term;
    int net_success;
    int net_nodeID;
    int net_ResponseID;

    int offset = 0;

    memcpy(&net_messageType, buffer + offset, sizeof(net_messageType));
    offset += sizeof(net_messageType);
    memcpy(&net_term, buffer + offset, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(&net_success, buffer + offset, sizeof(net_success));
    offset += sizeof(net_success);
    memcpy(&net_nodeID, buffer + offset, sizeof(net_nodeID));
    offset += sizeof(net_nodeID);
    memcpy(&net_ResponseID, buffer + offset, sizeof(net_ResponseID));

    messageType = ntohl(net_messageType);
    term = ntohl(net_term);
    success = ntohl(net_success);
    nodeID = ntohl(net_nodeID);
    ResponseID = ntohl(net_ResponseID);

}

void ResponseAppendEntry::Marshal(char *buffer){
    int net_messageType = htonl(messageType);
    int net_term = htonl(term);
    int net_success = htonl(success);
    int net_nodeID = htonl(nodeID);
    int net_ResponseID = htonl(ResponseID);
    int offset = 0;

    memcpy(buffer + offset, &net_messageType, sizeof(net_messageType));
    offset += sizeof(net_messageType);
    memcpy(buffer + offset, &net_term, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(buffer + offset, &net_success, sizeof(net_success));
    offset += sizeof(net_success);
    memcpy(buffer + offset, &net_nodeID, sizeof(net_nodeID));
    offset += sizeof(net_nodeID);
    memcpy(buffer + offset, &net_ResponseID, sizeof(net_ResponseID));
}

int ResponseAppendEntry::Size() {
    return sizeof(messageType) + sizeof(term) + sizeof(success) +
            sizeof(nodeID) + sizeof (ResponseID);
}

/* ----------------------Get private variables --------------------*/
int ResponseAppendEntry::Get_messageType(){
    return messageType;
}
int ResponseAppendEntry::Get_term(){
    return term;
}
int ResponseAppendEntry::Get_success(){
    return success;
}
int ResponseAppendEntry::Get_nodeID() {
    return nodeID;
}

int ResponseAppendEntry::Get_ResponseID() {
    return ResponseID;
}

void ResponseAppendEntry::Print(){
    ::Print_MessageType(messageType);
    std::cout << "term : " << term << '\n';
    std::cout << "success : " << success << '\n';
    std::cout << "nodeID : " << nodeID << '\n';
    std::cout << "-----------ResponseID : " << ResponseID << "--------------" << '\n';
    std::cout << "" << '\n';
}
