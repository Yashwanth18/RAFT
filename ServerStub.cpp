#include"ServerStub.h"

//return 1 on success and 0 on failure
int ServerStub:: Init(NodeInfo * node_info, int argc, char *argv[]){
  port = node_info -> port;
  node_id = node_info -> node_id;
  num_peers = node_info -> num_peers;

  total_socket_num = num_peers + 1;
  alive_fd_count = 0;
  for (int i = 1; i < num_peers + 1; i++)     {   pfds[i].fd = -1;  }

  Init_Listen_socket();
  return FillPeerServerInfo(argc, argv);
}

void ServerStub::Init_Listen_socket(){
  pfds[0].fd = ListenSocket.Init(port);
  pfds[0].events = POLLIN;      // Report ready to read on incoming connection
  alive_fd_count = 1;           //for the listening port
}

void ServerStub:: Accept_Connection(){
  int new_fd;
  struct sockaddr_in addr;
  unsigned int addr_size = sizeof(addr);

  //pfds[0].fd is the listening socket
  new_fd = accept(pfds[0].fd, (struct sockaddr *) &addr, &addr_size);
  if (new_fd < 0) perror ("accept");

  pfds[alive_fd_count].fd = new_fd;
  pfds[alive_fd_count].events = POLLIN;
  alive_fd_count ++;
}

void ServerStub:: Poll(int Poll_timeout){
  int poll_count = poll(pfds.data(), alive_fd_count, Poll_timeout); //number of ready to read files
  if( poll_count < 0 )   perror("poll");
  std::cout << "poll count: "<< poll_count << '\n';
}

//return the new file descriptor
int ServerStub::Connect_To(std::string ip, int port){
    int new_fd;
    struct sockaddr_in addr;

    new_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ < 0) {
        perror("ERROR: failed to create a socket");
        return -1;
    }

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    if (connect(fd_, (struct sockaddr *) &addr, sizeof(addr)) < 0){
      perror("connect: ");
      return -1;
    }

    fcntl(new_fd, F_SETFL, O_NONBLOCK);   //set socket to non-blocking
    return new_fd;
}

/* restart the timer:
if receive connection from a candidate
if receive heartbeat message from leader
if receive RequestVote message from candidate
*/
void ServerStub:: HandlePoll(ServerTimer * timer){

  for(int i = 0; i < total_socket_num; i++) {   //looping through file descriptors
      if (pfds[i].revents & POLLIN) {          //got ready-to-read from poll()

          if (i==0){      //events at the listening socket
            Accept_Connection();
          }

          else{
              int net_leader_id;
              char buf[4];
              int nbytes = recv(pfds[i].fd, buf, sizeof(net_leader_id), 0);

              if (nbytes <= 0){  //connection closed or error
                  close(pfds[i].fd);
                  pfds[i].fd = -1;

                  alive_fd_count --;  //use vector instead?
              }

              else{

                  memcpy(&net_leader_id, buf, sizeof(net_leader_id));
                  leader_id = ntohl(net_leader_id);
                  std::cout << leader_id << '\n';   //print the message

              }
          } //End events from old connection

          timer -> Restart();
      } //End got ready-to-read from poll()

  } // End looping through file descriptors
}


void ServerStub:: Broadcast_nodeID(){
  char buf[4];
  int net_leader_id;

  for (int i = 1; i < total_socket_num; i++){
      net_leader_id = htonl(node_id);
      memcpy(buf, &net_leader_id, sizeof(net_leader_id));
      send(pfds[i+1].fd, buf, sizeof(net_leader_id), 0);
  }

}

void ServerStub:: Election_Protocol(){
  for (int i = 0; i < num_peers; i++){
      pfds[i+1].fd = Connect_To(PeerServerInfo[i].IP, PeerServerInfo[i].port);
      Broadcast_nodeID();
  }


//return 0 on failure and 1 on success
int ServerStub::
FillPeerServerInfo(int argc, char *argv[]){

  for (int i = 1; i <= num_peers; i++){

    if (argc <= 3*i + 3){std::vector<NodeInfo> PeerServerInfo;
			std::cout << "not enough arguments" << std::endl;
			std::cout << "./server [port #] [unique ID] [# peers] \
                      (repeat [ID] [IP] [port #])	" << std::endl;
			return 0;
		}

		else{
			int unique_id = atoi(argv[3*i + 1]);
			std::string IP = argv[3*i + 2];
			int port = atoi(argv[3*i + 3]);

			ServerInfo peer_server_info {unique_id, IP, port};
			PeerServerInfo.push_back(peer_server_info);
		}

	} //END for loop

	return 1;
}





// // -------- receive request vote ----//
// int ServerStub::ReceiveRequestVote(RequestVote *requestVote) {
//     char buffer[32];
//     int socket_status;
//
//     socket_status = socket -> Recv(buffer, requestVote -> Size(), 0);
//     if (socket_status) {
//         requestVote-> Unmarshal(buffer);
//     }
//     return socket_status;
// }
//
// int ServerStub::SendRequestVote(RequestVote *requestVote) {
//     char buffer[32];
//     int socket_status;
//
//     requestVote->Marshal(buffer);
//     socket_status = socket->Send(buffer, requestVote->Size(), 0);
//     return socket_status;
// }
