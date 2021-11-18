#include"ServerStub.h"

//return number of successful votes
//int ServerStub::CountVote(){
//    int vote_count = 0;
//
//    int net_node_id_receive;
//    int node_id_receive;
//    char buf[4];
//
//    //Does not include the listening socket pfds[0]
//    int alive_socket = pfds.size();
//
//    for(int i = 1; i < alive_socket; i++) {     //looping through file descriptors
//        if (pfds[i].revents & POLLIN) {          //got ready-to-read from poll()
//            int nbytes = recv(pfds[i].fd, buf, sizeof(net_node_id_receive), 0);
//
//            if (nbytes <= 0){  //connection closed or error
//                close(pfds[i].fd);
//                alive_connection --;
//                pfds.erase(pfds.begin()+i);     //delete
//            }
//
//            else{        //got good data
//                memcpy(&net_node_id_receive, buf, sizeof(net_node_id_receive));
//                node_id_receive = ntohl(net_node_id_receive);
//                std::cout << "node_id_receive: " << node_id_receive << '\n';
//                vote_count ++;
//            }
//
//        } //End got ready-to-read from poll()
//    } // End looping through file descriptors
//
//    std::cout << "vote_count"<< vote_count << '\n';
//    return vote_count;
//}

// is this similar to receiving the request to vote
void ServerStub:: Handle_Poll(ServerTimer * timer){
  int net_node_id_receive;
  int node_id_receive;
  char buf[4];
  VoteResponse voteResponse;
  for(int i = 0; i < num_peers + 1; i++) {   //looping through file descriptors
      if (pfds[i].revents & POLLIN) {          //got ready-to-read from poll()

          if (i==0){ //events at the listening socket
//            Accept_Connection();
              continue;
          }

          else{ //events from established connection
              int nbytes = recv(pfds[i].fd, buf, sizeof(net_node_id_receive), 0);

              if (nbytes <= 0){  //connection closed or error
                  close(pfds[i].fd);
                  alive_connection --;
                  pfds.erase(pfds.begin()+i);     //delete
              }

              else{             //got good data
                  voteResponse.Unmarshal(buf);
                  voteResponse.Print();

                  //SendNodeID(pfds[i].fd);

              } //End got good data
          } //End events from established connection

          timer -> Restart();
      } //End got ready-to-read from poll()

  } // End looping through file descriptors
}


void ServerStub:: Connect_and_Send_RequestVoteRPC(NodeInfo * nodeInfo){
    RequestVote requestVote;
    requestVote.Set_RequestVote(nodeInfo->term, nodeInfo->node_id, nodeInfo->last_log_index, nodeInfo->last_log_term);
    for (int i = 0; i < num_peers; i++){
      int socket_status = Connect_To(PeerServerInfo[i].IP, PeerServerInfo[i].port);
        if(socket_status >= 0)
        {
            // request vote rpc
            SendRequestVoteRPC(pfds[i+1].fd,&requestVote);
        }


  }
}

//void ServerStub:: Broadcast_nodeID(){
//    for (int i = 0; i < num_peers; i++){
//        SendNodeID(pfds[i+1].fd);
//    }
//}

void ServerStub::SendRequestVoteRPC(int fd, RequestVote * requestVote) {
    char buf[32];
//    int net_node_id;

//    net_node_id = htonl(node_id);
//    memcpy(buf, &net_node_id, sizeof(net_node_id));
    requestVote->Marshal(buf);
    //to-do: do error handling error here
    send(fd, buf, sizeof(requestVote), 0);
}

//void ServerStub:: Accept_Connection(){
//  int new_fd;
//  struct sockaddr_in addr;
//  unsigned int addr_size = sizeof(addr);
//
//  //the listening socket is pfds[0].fd
//  new_fd = accept(pfds[0].fd, (struct sockaddr *) &addr, &addr_size);
//  if (new_fd < 0) perror ("accept");
//
//  Add_Socket_To_Poll(new_fd);
//}

void ServerStub:: Add_Socket_To_Poll(int new_fd){
    pollfd new_pfd;
    new_pfd.fd = new_fd;
    new_pfd.events = POLLIN;
    pfds.push_back(new_pfd);
}

int ServerStub:: Poll(int Poll_timeout){
    int poll_count = poll(pfds.data(), pfds.size(), Poll_timeout);
    if( poll_count < 0 )   perror("poll");
    //std::cout << "number of ready-to-read files: "<< poll_count << '\n';
    return poll_count;
}

int ServerStub:: Connect_To(std::string ip, int port){
//return the new file descriptor

//    int new_fd;
    ServerSocket serverSocket1;
    socketInfo *socketInfo;
//    struct sockaddr_in addr;
    struct socketInfo socketInfo1;
    socketInfo = serverSocket1.Init(ip,port);
//    if(socketInfo->socketStatus == 0)
//    {
//        return socketInfo;
//    }
    if(socketInfo->socketStatus == 1)
    {
        socketInfo1.fd = socketInfo->fd;
        socketInfo1.socketStatus = socketInfo->socketStatus;
        Add_Socket_To_Poll(socketInfo->fd);
        socket_info_vector.push_back(socketInfo1);

    }
    return socketInfo->socketStatus;
//    new_fd = socket(AF_INET, SOCK_STREAM, 0);
//    if (new_fd < 0) {
//        perror("ERROR: failed to create a socket");
//        return -1;
//    }
//
//    memset(&addr, '\0', sizeof(addr));
//    addr.sin_family = AF_INET;
//    addr.sin_addr.s_addr = inet_addr(ip.c_str());
//    addr.sin_port = htons(port);
//
//
//    //to-do: do error handling for connect. The same as Programming Assignment 2
//    if (connect(new_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0){
//      perror("connect: ");
//      return -1;
//    }
//
//    fcntl(new_fd, F_SETFL, O_NONBLOCK);   //set socket to non-blocking
//    return new_fd;
}

int ServerStub:: FillPeerServerInfo(int argc, char *argv[]){
//return 0 on failure and 1 on success

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

//void ServerStub:: Print_PeerServerInfo(){
//  for (int i = 0; i < num_peers; i++){
//    std::cout << "id: "<< PeerServerInfo[i].unique_id  << '\n';
//    std::cout << "IP: "<< PeerServerInfo[i].IP  << '\n';
//    std::cout << "Port: "<< PeerServerInfo[i].port  << '\n';
//    std::cout << "------------------"<< '\n';
//  }
//}

int ServerStub:: Init(NodeInfo * node_info, int argc, char *argv[]){
//return 1 on success and 0 on failure
  port = node_info -> port;
  node_id = node_info -> node_id;
  num_peers = node_info -> num_peers;

  Add_Socket_To_Poll(ListenSocket.Init(port));
  //for (int i = 1; i < num_total_sockets; i++)   { pfds[i].fd = -1;  }

  return FillPeerServerInfo(argc, argv);
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
