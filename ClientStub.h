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
	int OrderLaptop(CustomerRequest order, LaptopInfo * info);
	int Send_Connection_Descriptor();
	int ReadRecord (CustomerRequest order, CustomerRecord * record);
};


#endif // end of #ifndef __CLIENT_STUB_H__
