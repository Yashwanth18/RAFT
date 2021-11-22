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
    AppendEntryResponse appendEntryResponse;
    char buf[appendEntryRequest.Size()];

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

                  int success = 1;  // to-do: use Decide()
                  appendEntryResponse.Set(APPEND_ENTRY_RESPONSE, serverState -> currentTerm,
                                          success, nodeInfo -> node_id);

                  Send_AppendEntryResponse(&appendEntryResponse, pfds[i].fd);

              } /* End got good data */
            } /* End events from established connection */
            timer -> Restart();

        } /* End: got ready-to-read from poll() */
    } /*  End: looping through file descriptors */
}



bool ClientStub::Set_Result(ServerState *serverState, AppendEntryRequest *appendEntryRequest){
    bool result = false;

    int current_term =  serverState -> currentTerm;
    int log_length = serverState -> smr_log.size();

    int remote_term = appendEntryRequest -> Get_sender_term();
    int prevLogIndex = appendEntryRequest -> Get_prevLogIndex();

    if (remote_term < current_term){
        return false;
    }

    if (log_length < prevLogIndex){
        return false;
    }
//    else if()
//
//    }

    return result;
}





int ClientStub::Send_AppendEntryResponse(AppendEntryResponse *appendEntryResponse, int fd){
    int remain_size = appendEntryResponse -> Size();
    char buf[remain_size];
    int offset = 0;
    int bytes_written;

    appendEntryResponse -> Marshal(buf);

    while (remain_size > 0){
        /* to-do: error handling for send */
        bytes_written = send(fd, buf+offset, remain_size, 0);
        offset += bytes_written;
        remain_size -= bytes_written;
    }

    return 1;
}

///* Comparing the last_term and log length for the candidate node and the follower node */
//bool ClientStub::Compare_Log(NodeInfo * nodeInfo,RequestVote * requestVote) {
//
//    int candidate_last_log_term = requestVote -> Get_last_log_term();
//    int candidate_last_log_index = requestVote -> Get_last_log_index();
//
//    int node_last_log_term = nodeInfo -> lastLogTerm;
//    int node_last_log_index = nodeInfo -> lastLogIndex;
//
//    bool greater_last_log_term = candidate_last_log_term > node_last_log_term;
//    bool check_last_log_index = candidate_last_log_index >= node_last_log_index;
//
//    bool log_ok = false;
//
//    if (greater_last_log_term)
//    {
//        log_ok = true;
//    }
//
//    else if (candidate_last_log_term == node_last_log_term)
//    {
//        return check_last_log_index;
//    }
//
//    return log_ok;
//
//}
//




