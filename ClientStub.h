#ifndef __CLIENT_STUB_H__
#define __CLIENT_STUB_H__

#include <string>

#include "ClientSocket.h"
#include "Messages.h"
#include <cstring>
#include <arpa/inet.h>

class ClientStub {

 private:
  ClientSocket socket;

public:

	ClientStub();
	int Init(std::string ip, int port);
    int WriteRequest (CustomerRequest  customer_request, ResponseToCustomer * response_to_customer);
};


#endif // end of #ifndef __CLIENT_STUB_H__
