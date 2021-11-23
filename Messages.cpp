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
    logTerm = -1;
    opcode = -1;
    arg1 = -1;
    arg2 = -1;
    leaderCommit = -1;
}


void AppendEntryRequest:: Set(int _messageType, int _sender_term, int _leaderId,
                              int _prevLogTerm, int _prevLogIndex,
                              int _logTerm, int _opcode, int _arg1,
                              int _arg2, int _leaderCommit){

    messageType = _messageType;
    sender_term = _sender_term;
    leaderId = _leaderId;
    prevLogTerm = _prevLogTerm;
    prevLogIndex = _prevLogIndex;
    logTerm = _logTerm;
    opcode = _opcode;
    arg1 = _arg1;
    arg2 = _arg2;
    leaderCommit = _leaderCommit;
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


    messageType = ntohl(net_messageType);
    sender_term = ntohl(net_sender_term);
    leaderId = ntohl(net_leaderId);
    prevLogTerm = ntohl(net_prevLogTerm);
    prevLogIndex = ntohl(net_prevLogIndex);
    logTerm = ntohl(net_logTerm);
    opcode = ntohl(net_opcode);
    arg1 = ntohl(net_arg1);
    arg2 = ntohl(net_arg2);
    leaderCommit = ntohl(net_leaderCommit);

}

void AppendEntryRequest::Marshal(char *buffer){
    int net_messageType = htonl(messageType);
    
    int net_sender_term = htonl(sender_term);
    int net_leaderId = htonl(leaderId);
    int net_prevLogTerm = htonl(prevLogTerm);
    int net_prevLogIndex = htonl(prevLogIndex);
    
    int net_logTerm = htonl(logTerm);

    int net_opcode = htonl(opcode);
    int net_arg1 = htonl(arg1);

    int net_arg2 = htonl(arg2);
    int net_leaderCommit = htonl(leaderCommit);

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
}

int AppendEntryRequest::Size() {
    return sizeof(messageType) + sizeof(sender_term) + sizeof(leaderId) +
            sizeof(prevLogTerm) + sizeof(prevLogIndex) +
            sizeof(logTerm) + sizeof(opcode) + sizeof(arg1) + sizeof(arg2) + 
            sizeof (leaderCommit);
}

void AppendEntryRequest::Print(){
    std::cout << "--------------------------------" << '\n';
    std::cout << "messageType: " << messageType << '\n';
    std::cout << "sender_term : " << sender_term << '\n';
    std::cout << "leaderId : " << leaderId << '\n';
    std::cout << "prevLogTerm : " << prevLogTerm << '\n';
    std::cout << "prevLogIndex : " << prevLogIndex << '\n';
    std::cout << "logTerm : "<< logTerm << '\n';
    std::cout << "opcode : "<< opcode << '\n';
    std::cout << "arg1 : " << arg1 << '\n';
    std::cout << "arg2 : " << arg2 << '\n';
    std::cout << "leaderCommit : " << leaderCommit << '\n';
    std::cout << "--------------------------------" << '\n';

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
int AppendEntryRequest::Get_logTerm(){
    return logTerm;
}
int AppendEntryRequest::Get_opcode(){
    return opcode;
}
int AppendEntryRequest::  Get_arg1(){
    return arg1;
}
int AppendEntryRequest:: Get_arg2(){
    return arg2;
}
int AppendEntryRequest:: Get_leaderCommit(){
    return leaderCommit;
}


/*-------------------------------AppendEntryResponse class------------------*/
AppendEntryResponse::AppendEntryResponse()  {
    messageType = -1;
    term = -1;
    success = -1;
    nodeID = -1;
}


void AppendEntryResponse::
Set(int _messageType, int _term, int _success, int _nodeID){
    messageType = _messageType;
    term = _term;
    success = _success;
    nodeID = _nodeID;
}


void AppendEntryResponse::UnMarshal(char *buffer){
    int net_messageType;
    int net_term;
    int net_success;
    int net_nodeID;

    int offset = 0;

    memcpy(&net_messageType, buffer + offset, sizeof(net_messageType));
    offset += sizeof(net_messageType);
    memcpy(&net_term, buffer + offset, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(&net_success, buffer + offset, sizeof(net_success));
    offset += sizeof(net_success);
    memcpy(&net_nodeID, buffer + offset, sizeof(net_nodeID));

    messageType = ntohl(net_messageType);
    term = ntohl(net_term);
    success = ntohl(net_success);
    nodeID = ntohl(net_nodeID);

}

void AppendEntryResponse::Marshal(char *buffer){
    int net_messageType = htonl(messageType);
    int net_term = htonl(term);
    int net_success = htonl(success);
    int net_nodeID = htonl(nodeID);
    int offset = 0;

    memcpy(buffer + offset, &net_messageType, sizeof(net_messageType));
    offset += sizeof(net_messageType);
    memcpy(buffer + offset, &net_term, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(buffer + offset, &net_success, sizeof(net_success));
    offset += sizeof(net_success);
    memcpy(buffer + offset, &net_nodeID, sizeof(net_nodeID));
}

int AppendEntryResponse::Size() {
    return sizeof(messageType) + sizeof(term) + sizeof(success) + sizeof(nodeID);
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

void AppendEntryResponse::Print(){
    std::cout << "messageType: " << messageType << '\n';
    std::cout << "term : " << term << '\n';
    std::cout << "success : " << success << '\n';
    std::cout << "nodeID : " << nodeID << '\n';
}


/*---------------------Customer Request Class ------------*/

CustomerRequest::CustomerRequest() {
    messageType = CUSTOMER_REQUEST;
    logTerm = 0;
    opcode = -1;
    arg1 = -1;
    arg2 = -1;
}

void CustomerRequest::Set(int _messageType, int _log_term, int _opcode, int _arg1, int _arg2) {
    messageType = _messageType;
    logTerm = _log_term;
    opcode = _opcode;
    arg1 = _arg1;
    arg2 = _arg2;
}

int CustomerRequest::Size() {
    return sizeof (messageType) + sizeof (logTerm) + sizeof (opcode)
            + sizeof (arg1) + sizeof (arg2);
}

void CustomerRequest::Marshal(char *buffer) {
    int net_messageType = htonl(messageType);
    int net_log_term = htonl(logTerm);
    int net_opcode = htonl(opcode);
    int net_arg1 = htonl(arg1);
    int net_arg2 = htonl(arg2);
    int offset = 0;

    memcpy(buffer + offset, &net_messageType, sizeof(net_messageType));
    offset += sizeof(net_messageType);
    memcpy(buffer + offset, &net_log_term, sizeof(net_log_term));
    offset += sizeof(net_log_term);
    memcpy(buffer + offset, &net_opcode, sizeof(net_opcode));
    offset += sizeof(net_opcode);
    memcpy(buffer + offset, &net_arg1, sizeof(net_arg1));
    offset += sizeof(net_arg1);
    memcpy(buffer+offset, &net_arg2, sizeof(net_arg2));
}

void CustomerRequest::UnMarshal(char *buffer) {
    int net_messageType;
    int net_log_term;
    int net_opcode;
    int net_arg1;
    int net_arg2;

    int offset = 0;

    memcpy(&net_messageType, buffer + offset, sizeof(net_messageType));
    offset += sizeof(net_messageType);
    memcpy(&net_log_term, buffer + offset, sizeof(net_log_term));
    offset += sizeof(net_log_term);
    memcpy(&net_opcode, buffer + offset, sizeof(net_opcode));
    offset += sizeof(net_opcode);
    memcpy(&net_arg1, buffer + offset, sizeof(net_arg1));
    offset += sizeof (net_arg1);
    memcpy(&net_arg2, buffer+offset, sizeof(net_arg2));

    messageType = ntohl(net_messageType);
    logTerm = ntohl(net_log_term);
    opcode = ntohl(net_opcode);
    arg1 = ntohl(net_arg1);
    arg2 = ntohl(net_arg2);
}

void CustomerRequest::Print() {
    std::cout << "messageType: " << messageType << '\n';
    std::cout << "logTerm: " << logTerm << '\n';
    std::cout << "opcode: " << opcode << '\n';
    std::cout << "arg1: " << arg1 << '\n';
    std::cout << "arg2: " << arg2 << '\n';
}

/* -----------------Response to Customer Class-------------------- */

ResponseToCustomer::ResponseToCustomer() {
    nodeRole = -1;
    leaderID = -1;
}

void ResponseToCustomer::Set(int _node_role, int _leader_id) {
    nodeRole = _node_role;
    leaderID = _leader_id;
}

void ResponseToCustomer::Marshal(char *buffer) {
    int net_node_role = htonl(nodeRole);
    int net_leader_id = htonl(leaderID);
    int offset = 0;

    memcpy(buffer+offset, &net_node_role, sizeof (net_node_role));
    offset += sizeof(net_node_role);
    memcpy(buffer+offset, &net_leader_id, sizeof(net_leader_id));

}

void ResponseToCustomer::UnMarshal(char *buffer) {
    int net_node_role;
    int net_leader_id;
    int offset = 0;

    memcpy(&net_node_role, buffer + offset, sizeof(net_node_role));
    offset += sizeof(net_node_role);
    memcpy(&net_leader_id, buffer + offset, sizeof(net_leader_id));

    nodeRole = ntohl(net_node_role);
    leaderID = ntohl(net_leader_id);
}

int ResponseToCustomer::Size() {
    return sizeof (nodeRole) + sizeof (leaderID);
}

int ResponseToCustomer::Get_leader_id() {
    return leaderID;
}

int ResponseToCustomer::Get_node_role() {
    return nodeRole;
}