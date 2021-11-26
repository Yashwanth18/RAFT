//
// Created by Yashwanth reddy Beeravolu on 11/25/21.
//

#ifndef RAFT__CLIENTMAIN_H_
#define RAFT__CLIENTMAIN_H_


/* return 0 on failure and 1 on success */
int FillPeerServerInfo(int argc, char *argv[], std::vector<Peer_Info> *PeerServerInfo){

  int num_peers = atoi(argv[5]);

  for (int i = 1; i <= num_peers; i++){

    if (argc <= 3*i + 5){
      std::cout << "not enough arguments" << std::endl;
      std::cout << "./server [port_server #] [port_client #] [unique ID] [# peers] "
                   "(repeat [ID] [IP] [port #])	" << std::endl;
      return 0;
    }

    else{

      int unique_id = atoi(argv[3*i + 3]);
      std::string IP = argv[3*i + 4];
      int server_port = atoi(argv[3*i + 5]);

      /*
      std::cout << "Peer_id: " << unique_id << '\n';
      std::cout << "Ip: " << IP << '\n';
      std::cout << "server_port: " << server_port << '\n';
      */

      Peer_Info peer_server_info {unique_id, IP, server_port};
      PeerServerInfo -> push_back(peer_server_info);

    }

  } // END: for loop
  return 1;
}
#endif //RAFT__CLIENTMAIN_H_
