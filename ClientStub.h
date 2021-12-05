#ifndef __CLIENT_STUB_H__
#define __CLIENT_STUB_H__

#include <string>
#include "OutSocket.h"
#include "Messages.h"
#include <cstring>
#include <arpa/inet.h>

class ClientStub {
private:
	 OutSocket socket;
public:
	ClientStub();
	int Init(std::string ip, int port);
    void Close_Socket();

    // Fill leaderID and return socket_status
	int Order_LeaderID(CustomerRequest order, int *LeaderID);
    bool ReadRecord (CustomerRequest *order, CustomerRecord * record);
    int Order_WriteRequest(CustomerRequest *request);

};


#endif // end of #ifndef __CLIENT_STUB_H__
