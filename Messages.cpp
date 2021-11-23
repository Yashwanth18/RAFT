#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include "Messages.h"

/*-------------------------------AppendEntryRequest class------------------*/
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


void AppendEntryRequest::UnMarshal(char *buffer){
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
    std::cout << "" << '\n';
    // std::cout << "messageType: " << messageType << '\n';
//    std::cout << "sender_term : " << sender_term << '\n';
//    std::cout << "leaderId : " << leaderId << '\n';
//    std::cout << "prevLogTerm : " << prevLogTerm << '\n';
//    std::cout << "prevLogIndex : " << prevLogIndex << '\n';
//    std::cout << "logTerm : "<< logEntry.logTerm << '\n';
//    std::cout << "opcode : "<< logEntry.opcode << '\n';
//    std::cout << "arg1 : " << logEntry.arg1 << '\n';
//    std::cout << "arg2 : " << logEntry.arg2 << '\n';
//    std::cout << "leaderCommit : " << leaderCommit << '\n';
    std::cout << "-----------RequestID : " << RequestID << "--------------" << '\n';
}

/* ----------------------Get private variables --------------------*/
int AppendEntryRequest::Get_messageType(){
    return messageType;
}
int AppendEntryRequest::Get_sender_term(){
    return sender_term;
}
int AppendEntryRequest::Get_leaderId(){
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


/*-------------------------------AppendEntryResponse class------------------*/
AppendEntryResponse::AppendEntryResponse()  {
    messageType = -1;
    term = -1;
    success = -1;
    nodeID = -1;
    ResponseID = -1;
}


void AppendEntryResponse::
Set(int _messageType, int _term, int _success, int _nodeID, int _ResponseID){
    messageType = _messageType;
    term = _term;
    success = _success;
    nodeID = _nodeID;
    ResponseID = _ResponseID;
}


void AppendEntryResponse::UnMarshal(char *buffer){
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

void AppendEntryResponse::Marshal(char *buffer){
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

int AppendEntryResponse::Size() {
    return sizeof(messageType) + sizeof(term) + sizeof(success) +
            sizeof(nodeID) + sizeof (ResponseID);
}

/* ----------------------Get private variables --------------------*/
int AppendEntryResponse::Get_messageType(){
    return messageType;
}
int AppendEntryResponse::Get_term(){
    return term;
}
int AppendEntryResponse::Get_success(){
    return success;
}
int AppendEntryResponse::Get_nodeID() {
    return nodeID;
}

int AppendEntryResponse::Get_ResponseID() {
    return ResponseID;
}

void AppendEntryResponse::Print(){
    // std::cout << "\nmessageType: " << messageType << '\n';
    std::cout << "term : " << term << '\n';
    std::cout << "success : " << success << '\n';
    std::cout << "nodeID : " << nodeID << '\n';
    std::cout << "-----------ResponseID : " << ResponseID << "--------------" << '\n';
    std::cout << "" << '\n';
}
