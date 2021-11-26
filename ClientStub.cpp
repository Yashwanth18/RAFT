#include "ClientStub.h"
#include "iostream"
ClientStub::ClientStub() {}

int ClientStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}

int ClientStub::WriteRequest(CustomerRequest customer_request, ResponseToCustomer *response_to_customer) {
  char buffer[128];
  int size;
  int status;

  customer_request.Marshal(buffer);
  size = customer_request.Size();
  status = socket.Send(buffer, size, 0);

  if (status == 1) {

    size = response_to_customer -> Size();
    status = socket.Recv(buffer, size, 0); // check for the response, if it's coming from either
                                                // leader or the follower

    if (status == 1) {

      response_to_customer -> UnMarshal(buffer);
    }
  }
  return status;
}