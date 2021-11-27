#include "ClientStub.h"
#define VOTE_GRANTED 1
#define VOTE_DENIED 0

void ClientStub:: Init(int port){
    Add_Socket_To_Poll(ListenSocket.Init(port));
}

void ClientStub:: Add_Socket_To_Poll(int new_fd){
    pollfd new_pfd;
    new_pfd.fd = new_fd;
    new_pfd.events = POLLIN;
    pfds.push_back(new_pfd);
}

void ClientStub:: Accept_Connection(){
  int new_fd;
  struct sockaddr_in addr;
  unsigned int addr_size = sizeof(addr);

  /* pfds[0].fd is the the listening socket */
  new_fd = accept(pfds[0].fd, (struct sockaddr *) &addr, &addr_size);
  if (new_fd < 0) perror ("accept");

  Add_Socket_To_Poll(new_fd);
  std::cout << "Accepted Connection" << '\n';
}

int ClientStub:: Poll(int Poll_timeout){
    int poll_count = poll(pfds.data(), pfds.size(), Poll_timeout);
    if( poll_count < 0 )   perror("poll");
    return poll_count;
}

/* functionalities include:
  ~ non-blocking receive RequestVoteRPC
*/
void ClientStub::
Handle_Follower_Poll(ServerState *serverState, ClientTimer *timer, NodeInfo *nodeInfo){
    AppendEntryRequest appendEntryRequest;
    ResponseAppendEntry ResponseAppendEntry;
    char buf[appendEntryRequest.Size()];
    int success_appendEntry;
    int num_alive_sockets = pfds.size();

    for(int i = 0; i < num_alive_sockets; i++) {   /* looping through file descriptors */
        if (pfds[i].revents & POLLIN) {            /* got ready-to-read from poll() */

            if (i==0){ /* events at the listening socket */
                Accept_Connection();
            }

            else { /* events from established connection */

                int nbytes = recv(pfds[i].fd, buf, sizeof(appendEntryRequest), 0);

                if (nbytes <= 0){  /* connection closed or error */
                  close(pfds[i].fd);
                  pfds.erase(pfds.begin()+i);     /* delete */
                }
                else{             /* got good data */
                    appendEntryRequest.UnMarshal(buf);
                    appendEntryRequest.Print();

                    timer -> Print_elapsed_time();

//                    std::cout << "\n###### Before func: " << '\n';
//                    std::cout << "committed index: " << serverState -> commitIndex << '\n';
                    Set_CommitIndex(&appendEntryRequest, serverState);
//                    std::cout << "\n###### After func: " << '\n';
//                    std::cout << "committed index: " << serverState -> commitIndex << '\n';


                    std::cout << "\n###### Before: " << '\n';
                    Print_Log(serverState);

                    success_appendEntry = Set_Result(serverState, &appendEntryRequest);

                    std::cout << "\n###### After: " << '\n';
                    Print_Log(serverState);

                    int ResponseID = appendEntryRequest.Get_RequestID() + 1;
                    ResponseAppendEntry.Set(APPEND_ENTRY_RESPONSE, serverState -> currentTerm,
                                          success_appendEntry, nodeInfo -> node_id, ResponseID);

                    Send_ResponseAppendEntry(&ResponseAppendEntry, pfds[i].fd);

              } /* End got good data */
            } /* End events from established connection */
            timer -> Restart();

        } /* End: got ready-to-read from poll() */
    } /*  End: looping through file descriptors */
}


/* If leaderCommit > commitIndex,
 * Set commitIndex = min(leaderCommit, index of last new entry) */
void ClientStub::
Set_CommitIndex(AppendEntryRequest *appendEntryRequest, ServerState * serverState) {

    /* local state */
    int local_commitIndex = serverState -> commitIndex;
    int local_log_length = serverState -> smr_log.size();

    /* from the remote side */
    int leaderCommit = appendEntryRequest -> Get_leaderCommit();

    if (leaderCommit > local_commitIndex){
        if (leaderCommit > local_log_length){
            serverState -> commitIndex = local_log_length;
        }
        else{
            serverState -> commitIndex = leaderCommit;
        }
    }
}

/* to-do: Ideally, should return false to the leader first before modifying local log to
 * optimize latency */
bool ClientStub::Set_Result(ServerState *serverState, AppendEntryRequest *appendEntryRequest){
    /* local state */
    int local_term = serverState -> currentTerm;
    int local_log_length = serverState -> smr_log.size();
    int local_prevLogTerm;
    std::vector<LogEntry>::iterator iter = serverState -> smr_log.begin();

    /* from the remote side */
    int remote_term = appendEntryRequest -> Get_sender_term();
    int remote_prevLogIndex = appendEntryRequest -> Get_prevLogIndex();
    int remote_prevLogTerm = appendEntryRequest -> Get_prevLogTerm();
    LogEntry remote_logEntry = appendEntryRequest -> Get_LogEntry();


    /* Reply false the remote node is stale */
    if (remote_term < local_term){
        return false;
    }

    /* Reply false if log does not contain an entry at prevLogIndex whose term matches prevLogTerm */
    if (local_log_length - 1 < remote_prevLogIndex){
        return false; // no element in local smr_log
    }

    else{   // if there is an entry in the local log at remote_prevLogIndex
        local_prevLogTerm = serverState -> smr_log.at(remote_prevLogIndex).logTerm;

        if (local_prevLogTerm != remote_prevLogTerm){ // check if conflicting prev log entry

            if (local_log_length > 1){  // keep first log to prevent index out of bound error
                /* erase conflicting log */
                serverState -> smr_log.erase(iter + remote_prevLogIndex, iter + local_log_length);
            }
            return false;
        }

        else{
            // check for conflicting entry at the last index of local smr_log
            if (remote_logEntry.logTerm == serverState -> smr_log.back().logTerm){
                return true;     // Already in the smr_log !
            }

            serverState -> smr_log.push_back(remote_logEntry);
            return true;
        }
    }



    // serverState -> smr_log.erase(iter + local_log_length - 1);
}

void ClientStub::Print_Log(ServerState *serverState){
    LogEntry logEntry;
    for (int i = 0; i < serverState -> smr_log.size(); i++){
        logEntry = serverState -> smr_log.at(i);

        std::cout << "-----Log entry number: "<< i << "-----" << '\n';
        std::cout << "logTerm : "<< logEntry.logTerm << '\n';
    }
}





int ClientStub::Send_ResponseAppendEntry(ResponseAppendEntry *ResponseAppendEntry, int fd){
    int remain_size = ResponseAppendEntry -> Size();
    char buf[remain_size];
    int offset = 0;
    int bytes_written;

    ResponseAppendEntry -> Marshal(buf);

    while (remain_size > 0){
        /* to-do: error handling for send */
        bytes_written = send(fd, buf+offset, remain_size, 0);
        offset += bytes_written;
        remain_size -= bytes_written;
    }

    return 1;
}





