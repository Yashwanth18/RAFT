#include "ClientStub.h"
// // -------- receive request vote ----//
// int ClientStub::ReceiveRequestVote(RequestVote *requestVote) {
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
// int ClientStub::SendRequestVote(RequestVote *requestVote) {
//     char buffer[32];
//     int socket_status;
//
//     requestVote->Marshal(buffer);
//     socket_status = socket->Send(buffer, requestVote->Size(), 0);
//     return socket_status;
// }

void ClientStub:: Handle_Follower_Poll(ClientTimer * timer){
  int net_node_id_receive;
  int node_id_receive;
  char buf[4];

  for(int i = 0; i < num_peers + 1; i++) {   //looping through file descriptors
      if (pfds[i].revents & POLLIN) {          //got ready-to-read from poll()

          if (i==0){ //events at the listening socket
            Accept_Connection();
          }

          else{ //events from established connection
              int nbytes = recv(pfds[i].fd, buf, sizeof(net_node_id_receive), 0);

              if (nbytes <= 0){  //connection closed or error
                  close(pfds[i].fd);
                  alive_connection --;
                  pfds.erase(pfds.begin()+i);     //delete
              }

              else{             //got good data
                  memcpy(&net_node_id_receive, buf, sizeof(net_node_id_receive));
                  node_id_receive = ntohl(net_node_id_receive);
                  std::cout << "node_id_receive: " << node_id_receive << '\n';

                  //to-do: implement a real ReplyVoteRPC
                  SendNodeID(pfds[i].fd);

              } //End got good data
          } //End events from established connection

          timer -> Restart();
      } //End got ready-to-read from poll()

  } // End looping through file descriptors
}

void ClientStub:: SendNodeID(int fd){
    char buf[4];
    int net_node_id;

    net_node_id = htonl(node_id);
    memcpy(buf, &net_node_id, sizeof(net_node_id));

    //to-do: do error handling error here
    send(fd, buf, sizeof(net_node_id), 0);
}

void ClientStub:: Accept_Connection(){
  int new_fd;
  struct sockaddr_in addr;
  unsigned int addr_size = sizeof(addr);

  //the listening socket is pfds[0].fd
  new_fd = accept(pfds[0].fd, (struct sockaddr *) &addr, &addr_size);
  if (new_fd < 0) perror ("accept");

  Add_Socket_To_Poll(new_fd);
}

void ClientStub:: Add_Socket_To_Poll(int new_fd){
    pollfd new_pfd;
    new_pfd.fd = new_fd;
    new_pfd.events = POLLIN;
    pfds.push_back(new_pfd);
}

int ClientStub:: Poll(int Poll_timeout){
    int poll_count = poll(pfds.data(), pfds.size(), Poll_timeout);
    if( poll_count < 0 )   perror("poll");
    //std::cout << "number of ready-to-read files: "<< poll_count << '\n';
    return poll_count;
}

int ClientStub:: FillPeerClientInfo(int argc, char *argv[]){
//return 0 on failure and 1 on success

  for (int i = 1; i <= num_peers; i++){

    if (argc <= 3*i + 3){
			std::cout << "not enough arguments" << std::endl;
			std::cout << "./Client [port #] [unique ID] [# peers] \
                      (repeat [ID] [IP] [port #])	" << std::endl;
			return 0;
		}

		else{
			int unique_id = atoi(argv[3*i + 1]);
			std::string IP = argv[3*i + 2];
			int port = atoi(argv[3*i + 3]);

			Peer_Info peer_Client_info {unique_id, IP, port};
			PeerClientInfo.push_back(peer_Client_info);
		}

	} //END for loop
	return 1;
}

void ClientStub:: Print_PeerClientInfo(){
  for (int i = 0; i < num_peers; i++){
    std::cout << "id: "<< PeerClientInfo[i].unique_id  << '\n';
    std::cout << "IP: "<< PeerClientInfo[i].IP  << '\n';
    std::cout << "Port: "<< PeerClientInfo[i].port  << '\n';
    std::cout << "------------------"<< '\n';
  }
}

int ClientStub:: Init(NodeInfo * node_info, int argc, char *argv[]){
//return 1 on success and 0 on failure
  port = node_info -> port;
  node_id = node_info -> node_id;
  num_peers = node_info -> num_peers;

  Add_Socket_To_Poll(ListenSocket.Init(port));
  //for (int i = 1; i < num_total_sockets; i++)   { pfds[i].fd = -1;  }

  return FillPeerClientInfo(argc, argv);
}
