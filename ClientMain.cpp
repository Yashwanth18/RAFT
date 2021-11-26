#include <array>
#include <iostream>

#include "ClientStub.h"
#include "Messages.h"
#include "ClientMain.h"
int main(int argc, char *argv[]) {

    std::string ip;
    int port;
    int unique_id;
    int request_type;
    int opcode;
    int arg1;
    int arg2;
  std::vector<Peer_Info> PeerServerInfo;
  if (argc < 7) {
    std::cout << "not enough arguments" << std::endl;
    std::cout << argv[0] << "[ip] [port #] [unique id] ";
    std::cout << " [request type 1 or 2] [opcode arg1 arg2]" << std::endl;
    return 0;
  }

  if (!FillPeerServerInfo(argc, argv, &PeerServerInfo) ){
    return 0;
  }





  ip = argv[1];
    port = atoi(argv[2]);
    unique_id = atoi(argv[3]);
    request_type = atoi(argv[4]);
    opcode = 1;
    arg1 = 2;
    arg2 = 3;

    ClientStub stub;
    stub.Init(ip, port);


    ResponseToCustomer  response_to_customer;

    CustomerRequest customer_request;
    customer_request.Set(CUSTOMER_REQUEST, WRITE, unique_id, opcode, arg1, arg2);

    if(request_type == WRITE)
    {
      stub.WriteRequest(customer_request, & response_to_customer);
      int leader_id = response_to_customer.Get_leader_id();

      if (response_to_customer.Get_sender_type() == SENDER_FOLLOWER) {

        for (auto & element : PeerServerInfo) {

          if (element.unique_id == leader_id) {

            std::cout << "ip_address of the leader is: " << element.IP << '\n';
            std::cout << "port number of the leader is : " << element.port << '\n';

          }
        }
      }

      else if (response_to_customer.Get_sender_type() == SENDER_LEADER){
        // just prints a message stating contacting with the leader
      }
    }



}
