#include"ServerStub.h"

/*
Initialize a non-blocking listening port and fill peer server info
Return 1 on success and 0 on failure
*/
int ServerStub:: Init(NodeInfo * node_info, int argc, char *argv[]){

  port = node_info -> port;
  node_id = node_info -> node_id;
  num_peers = node_info -> num_peers;

  Add_Socket_To_Poll(ListenSocket.Init(port));

  return FillPeerServerInfo(argc, argv);
}

void ServerStub:: Add_Socket_To_Poll(int new_fd){
    pollfd new_pfd;
    new_pfd.fd = new_fd;
    new_pfd.events = POLLIN;
    pfds.push_back(new_pfd);
}

void ServerStub::Connect_Follower(){
    for (int i = 0; i < num_peers; i++){
        int new_fd = Connect_To(PeerServerInfo[i].IP, PeerServerInfo[i].port);
        Add_Socket_To_Poll(new_fd);
    }
}



void ServerStub::Send_RequestVoteRPC(NodeInfo * node_info){
    RequestVote requestVote;
    int term = node_info -> term;
    int candidateId = node_info -> node_id;
    int lastLogIndex = node_info -> lastLogIndex;
    int lastLogTerm = node_info -> lastLogTerm;

    requestVote.Set(term, candidateId, lastLogIndex, lastLogTerm);
    for (int i = 1; i < pfds.size(); i++){
        SendRequestVote(&requestVote, pfds[i].fd);
    }

}

int ServerStub::SendRequestVote(RequestVote *requestVote, int fd) {
    int remain_size = requestVote -> Size();
    char buf[remain_size];
    //int socket_status;
    int offset = 0;
    int bytes_written;

    requestVote->Marshal(buf);

    while (remain_size > 0){
        bytes_written = send(fd, buf+offset, remain_size, 0);
        offset += bytes_written;
        remain_size -= bytes_written;
    }

    return 1;   //to-do: fix this with socket_status
}



/*return the new file descriptor*/
int ServerStub:: Connect_To(std::string ip, int port){
    int new_fd;
    struct sockaddr_in addr;

    new_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (new_fd < 0) {
        perror("ERROR: failed to create a socket");
        return -1;
    }

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);


    //to-do: do error handling for connect. The same as Programming Assignment 2
    if (connect(new_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0){
      perror("connect: ");
      return -1;
    }

    fcntl(new_fd, F_SETFL, O_NONBLOCK);   //set socket to non-blocking
    return new_fd;
}


void ServerStub:: Accept_Connection(){
  int new_fd;
  struct sockaddr_in addr;
  unsigned int addr_size = sizeof(addr);

  //the listening socket is pfds[0].fd
  new_fd = accept(pfds[0].fd, (struct sockaddr *) &addr, &addr_size);
  if (new_fd < 0) perror ("accept");

  Add_Socket_To_Poll(new_fd);
}



int ServerStub:: Poll(int poll_timeout){
    int poll_count = poll(pfds.data(), pfds.size(), poll_timeout);
    if( poll_count < 0 )   perror("poll");
    return poll_count;
}

/* functionalities include:
  ~ non-blocking receive VoteResponse
*/
void ServerStub:: Handle_Poll(int * num_votes){
    VoteResponse voteResponse;
    char buf[voteResponse.Size()];
    int num_alive_sockets = pfds.size();

    for(int i = 0; i < num_alive_sockets; i++) {   /* looping through file descriptors */
        if (pfds[i].revents & POLLIN) {            /* got ready-to-read from poll() */

            if (i==0){                             /* events at the listening socket */
                Accept_Connection();
            }

            else{                                   /* events from established connection */
                int nbytes = recv(pfds[i].fd, buf, sizeof(voteResponse), 0);

                if (nbytes <= 0){                           /* connection closed or error */
                    close(pfds[i].fd);
                    pfds.erase(pfds.begin()+i);     /* delete */
                }

                else{                                       /* got good data */
                    voteResponse.Unmarshal(buf);
                    voteResponse.Print();

                    /* Need a mechanism to check which nodes have voted. */
                    if (voteResponse.Get_voteGranted()){
                        (* num_votes) ++;
                    }
                }                                           /* End got good data */
            }                     /* End events from established connection */
        }                    /* End got ready-to-read from poll() */
    }                    /* End looping through file descriptors */
}



/* return 0 on failure and 1 on success */
int ServerStub:: FillPeerServerInfo(int argc, char *argv[]){

  for (int i = 1; i <= num_peers; i++){

    if (argc <= 3*i + 3){
			std::cout << "not enough arguments" << std::endl;
			std::cout << "./server [port #] [unique ID] [# peers] \
                      (repeat [ID] [IP] [port #])	" << std::endl;
			return 0;
		}

		else{
			int unique_id = atoi(argv[3*i + 1]);
			std::string IP = argv[3*i + 2];
			int port = atoi(argv[3*i + 3]);

			Peer_Info peer_server_info {unique_id, IP, port};
			PeerServerInfo.push_back(peer_server_info);
		}

	} //END for loop
	return 1;
}


/* For debugging */
void ServerStub:: Print_PeerServerInfo(){
  for (int i = 0; i < num_peers; i++){
    std::cout << "id: "<< PeerServerInfo[i].unique_id  << '\n';
    std::cout << "IP: "<< PeerServerInfo[i].IP  << '\n';
    std::cout << "Port: "<< PeerServerInfo[i].port  << '\n';
    std::cout << "------------------"<< '\n';
  }
}


/* Append Entries RPC which appends the log messa*/
void ServerStub::Send_AppendEntriesRPC(NodeInfo *node_info) {
    AppendEntries appendEntries;
    int term = node_info->term;
    int opcode = node_info->opcode;
    int arg1 = node_info->arg1;
    int arg2 = node_info->arg2;
    appendEntries.Set_AppendEntries(term, opcode, arg1, arg2);
    for (int i = 1; i < pfds.size(); i++){
        Send_AppendEntries(&appendEntries, pfds[i].fd);
    }
}

/* Leader node placing the replication request */
int ServerStub::Send_AppendEntries(AppendEntries *appendEntries, int fd) {
    int remain_size = appendEntries -> size();
    char buf[remain_size];
    //int socket_status;
    int offset = 0;
    int bytes_written;

    appendEntries->Marshal(buf);

    while (remain_size > 0){
        bytes_written = send(fd, buf+offset, remain_size, 0);
        offset += bytes_written;
        remain_size -= bytes_written;
    }
    std::cout << "remain_size: "<< remain_size << '\n';
    return 1;   //to-do: fix this with socket_status
}
