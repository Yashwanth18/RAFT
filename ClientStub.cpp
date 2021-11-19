#include "ClientStub.h"

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

  //pfds[0].fd is the the listening socket
  new_fd = accept(pfds[0].fd, (struct sockaddr *) &addr, &addr_size);
  if (new_fd < 0) perror ("accept");

  Add_Socket_To_Poll(new_fd);
}
/*functionality:
  non-blocking recvieve RequestVoteRPC
*/
void ClientStub:: Handle_Follower_Poll(ClientTimer * timer){
    RequestVote requestVote;
    int num_alive_sockets = pfds.size();


    for(int i = 0; i < num_alive_sockets; i++) {   //looping through file descriptors
        if (pfds[i].revents & POLLIN) {            //got ready-to-read from poll()

            if (i==0){ //events at the listening socket
                Accept_Connection();
            }

            else{ //events from established connection
              char buf[requestVote.Size()];
              int nbytes = recv(pfds[i].fd, buf, sizeof(requestVote), 0);

              if (nbytes <= 0){  //connection closed or error
                  close(pfds[i].fd);
                  pfds.erase(pfds.begin()+i);     //delete
              }

              else{             //got good data

                  requestVote.Unmarshal(buf);
                  requestVote.Print();
                  //to-do: implement a real ReplyVoteRPC

              } //End got good data
            } //End events from established connection

            timer -> Restart();
        } //End got ready-to-read from poll()
    } // End looping through file descriptors
}




int ClientStub:: Poll(int Poll_timeout){
    int poll_count = poll(pfds.data(), pfds.size(), Poll_timeout);
    if( poll_count < 0 )   perror("poll");
    return poll_count;
}
