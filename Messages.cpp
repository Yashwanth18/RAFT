#include <cstring>
#include <arpa/inet.h>
#include <iostream>

#include "Messages.h"

/*----------------------Class : ServerState----------------------------------*/
ServerState::ServerState(int num_peers, int _role) {
    /* Persistent state on all servers: Updated on stable storage before responding to RPCs */
    currentTerm = 0;
    votedFor = -1;
    LogEntry logEntry {-1, -1, -1, -1};
    smr_log.push_back(logEntry);

    /* volatile state on leaders (Reinitialized after Raft) */
    for (int i = 0; i < num_peers; i++){
        matchIndex.push_back(0);
        nextIndex.push_back(1); // initialized to be last log index + 1
    }

    /* volatile state on all servers */
    commitIndex = 0;
    lastApplied = 0;

    numVotes = 0;            /* 1 because always vote for oneself */
    leaderId = -1;
    role = _role;           /* for testing purpose only! */
}
void ServerState::SetRole(int _role){
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock

    role = _role;

    if (_role == FOLLOWER){
        std::cout << "Resigning to be a follower now!" << '\n';
    }
    else if (_role == CANDIDATE){
        std::cout << "Becoming a candidate now!" << '\n';
    }
    ul.unlock();     // unlock
}

void ServerState::Set_nodeTerm(int _term) {
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    currentTerm = _term;
}
void ServerState::SetVotedFor(int id){
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    votedFor = id;
}

void ServerState::Set_commitIndex(int _index){
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    commitIndex = _index;
}
void ServerState::Set_lastApplied(int _index){
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    lastApplied = _index;
}
void ServerState::Set_leaderID(int id){
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    leaderId = id;
}

int ServerState::Get_nodeTerm() {
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    return currentTerm;
}

int ServerState::GetRole() {
    int role_;
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    role_ = role;
    return role_;
}

int ServerState::Get_numVotes() {
    int num_votes;
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    num_votes = numVotes;
    return num_votes;
}

int ServerState::Get_lastApplied(){
    int last_applied;
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    last_applied = lastApplied;
    return last_applied;
}
int ServerState::Get_VotedFor(){
    int voted_for;
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    voted_for = votedFor;
    return voted_for;
}
int ServerState::Get_leaderID(){
    int leader_id;
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    leader_id = leaderId;
    return leader_id;
}

int ServerState::Get_commitIndex(bool Atomic) {
    int leader_id;
    if (Atomic){
        std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
        ul.lock();        // lock
        leader_id = leaderId;
    }
    else{
        leader_id = leaderId;
    }

    return leader_id;
}

void ServerState::Increment_numVote() {
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    numVotes ++;
}

void ServerState::Increment_nextIndex(int peer_index){
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    nextIndex[peer_index] ++;
}
void ServerState::Decrement_nextIndex(int peer_index){
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    nextIndex[peer_index] --;
}

void ServerState::Set_matchIndex(int peer_index, int log_index){
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    matchIndex[peer_index] = log_index;
}

void ServerState::NewElection(int self_nodeId) {
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);

    ul.lock();        // lock
    currentTerm ++;
    numVotes = 1;
    votedFor = self_nodeId; // vote for itself
    ul.unlock();     // unlock
}

void ServerState::Become_Leader(int self_nodeID) {
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    role = LEADER;
    leaderId = self_nodeID;
    std::cout << "Becoming a leader now!" << '\n';
}


LogEntry ServerState::Get_LogEntry(int index){
    LogEntry logEntry = {0, -1, -1, -1};
    std::unique_lock<std::mutex> ul(this -> lck, std::defer_lock);
    ul.lock();        // lock
    logEntry = smr_log.at(index);
    return logEntry;
}



/*---------------------------------------Leader Raft----------------------------------*/
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
    std::cout << "-----------VoteRequest--------------" << '\n';
    std::cout << "term: " << term << '\n';
    std::cout << "candidateId: " << candidateId << '\n';
    std::cout << "lastLogIndex: "<< lastLogIndex << '\n';
    std::cout << "lastLogTerm: "<< lastLogTerm << '\n';
    std::cout<<"" <<'\n';
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

}


void AppendEntryRequest:: Set( int _sender_term, int _leaderId,
                              int _prevLogTerm, int _prevLogIndex,
                              LogEntry * _logEntry, int _leaderCommit){

    sender_term = _sender_term;
    leaderId = _leaderId;
    prevLogTerm = _prevLogTerm;
    prevLogIndex = _prevLogIndex;
    logEntry = *_logEntry;
    leaderCommit = _leaderCommit;

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


    sender_term = ntohl(net_sender_term);
    leaderId = ntohl(net_leaderId);
    prevLogTerm = ntohl(net_prevLogTerm);
    prevLogIndex = ntohl(net_prevLogIndex);
    logEntry.logTerm = ntohl(net_logTerm);
    logEntry.opcode = ntohl(net_opcode);
    logEntry.arg1 = ntohl(net_arg1);
    logEntry.arg2 = ntohl(net_arg2);
    leaderCommit = ntohl(net_leaderCommit);

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

}

int AppendEntryRequest::Size() {
    return  sizeof(sender_term) + sizeof(leaderId) +
            sizeof(prevLogTerm) + sizeof(prevLogIndex) +
           sizeof(logEntry )+ sizeof (leaderCommit);
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



/*----------------------------ResponseAppendEntry class------------------*/
ResponseAppendEntry::ResponseAppendEntry()  {
    term = -1;
    success = -1;
    Heartbeat = -1;
}


void ResponseAppendEntry::
Set( int _term, int _success, int _Heartbeat){
    term = _term;
    success = _success;
    Heartbeat = _Heartbeat;
}


void ResponseAppendEntry::Unmarshal(char *buffer){

    int net_term;
    int net_success;

    int net_Heartbeat;

    int offset = 0;


    memcpy(&net_term, buffer + offset, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(&net_success, buffer + offset, sizeof(net_success));
    offset += sizeof(net_success);
    memcpy(&net_Heartbeat, buffer + offset, sizeof(net_Heartbeat));

    term = ntohl(net_term);
    success = ntohl(net_success);
    Heartbeat = ntohl(net_Heartbeat);

}

void ResponseAppendEntry::Marshal(char *buffer){
    int net_term = htonl(term);
    int net_success = htonl(success);
    int net_Heartbeat = htonl(Heartbeat);
    int offset = 0;


    memcpy(buffer + offset, &net_term, sizeof(net_term));
    offset += sizeof(net_term);
    memcpy(buffer + offset, &net_success, sizeof(net_success));
    offset += sizeof(net_success);
    memcpy(buffer + offset, &net_Heartbeat, sizeof(net_Heartbeat));
}

int ResponseAppendEntry::Size() {
    return sizeof(term) + sizeof(success) + sizeof (Heartbeat);
}

/* ----------------------Get private variables --------------------*/

int ResponseAppendEntry::Get_term(){
    return term;
}
int ResponseAppendEntry::Get_success(){
    return success;
}

int ResponseAppendEntry::Get_Heartbeat() {
    return Heartbeat;
}

void ResponseAppendEntry::Print(){

    std::cout << "\nterm : " << term << '\n';
    std::cout << "success : " << success << '\n';
    std::cout << "Heartbeat : " << Heartbeat << "" << '\n';
    std::cout << "" << '\n';
}



/*---------------------------Class: CustomerRequest---------------------------------*/
CustomerRequest::CustomerRequest() {
    customer_id = -1;
    order_number = -1;
    request_type = -1;
}

void CustomerRequest::SetOrder(int id, int number, int type) {
    customer_id = id;
    order_number = number;
    request_type = type;
}

int CustomerRequest::GetCustomerId() { return customer_id; }
int CustomerRequest::GetOrderNumber() { return order_number; }
int CustomerRequest::GetRequestType() { return request_type; }

int CustomerRequest::Size() {
    return sizeof(customer_id) + sizeof(order_number) + sizeof(request_type);
}

void CustomerRequest::Marshal(char *buffer) {
    int net_customer_id = htonl(customer_id);
    int net_order_number = htonl(order_number);
    int net_request_type = htonl(request_type);
    int offset = 0;

    memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);
    memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
    offset += sizeof(net_order_number);
    memcpy(buffer + offset, &net_request_type, sizeof(net_request_type));
}

void CustomerRequest::Unmarshal(char *buffer) {
    int net_customer_id;
    int net_order_number;
    int net_request_type;
    int offset = 0;
    memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);
    memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
    offset += sizeof(net_order_number);
    memcpy(&net_request_type, buffer + offset, sizeof(net_request_type));

    customer_id = ntohl(net_customer_id);
    order_number = ntohl(net_order_number);
    request_type = ntohl(net_request_type);
}

bool CustomerRequest::IsValid() {
    return (request_type != -1);
}

void CustomerRequest::Print() {
    std::cout << "customer_id: " << customer_id << " ";
    std::cout << "order_number: " << order_number << " ";
    std::cout << "request_type: " << request_type << std::endl;
}


/*-------------------------CustomerRecord Class-------------------------------*/
CustomerRecord::CustomerRecord() {
    customer_id = -1;
    last_order = -1;
}

bool CustomerRecord::IsValid() {
    return (customer_id != -1);
}
void CustomerRecord::Print() {
    std::cout << "id " << customer_id << " ";
    std::cout << "last_order " << last_order << std::endl;
}

void CustomerRecord::SetCustomerId(int id) { customer_id = id; }
void CustomerRecord::SetLastOrder(int order_number) { last_order = order_number; }

int CustomerRecord::GetCustomerId() { return customer_id; }
int CustomerRecord::GetLastOrder() { return last_order; }

int CustomerRecord::Size() {
    return sizeof(customer_id) + sizeof(last_order) ;
}


void CustomerRecord::Marshal(char *buffer) {
    int net_customer_id = htonl(customer_id);
    int net_last_order= htonl(last_order);
    int offset = 0;

    memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);

    memcpy(buffer + offset, &net_last_order, sizeof(net_last_order));
}

void CustomerRecord::Unmarshal(char *buffer) {
    int net_customer_id;
    int net_last_order;

    int offset = 0;

    memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);
    memcpy(&net_last_order, buffer + offset, sizeof(net_last_order));

    customer_id = ntohl(net_customer_id);
    last_order = ntohl(net_last_order);

}