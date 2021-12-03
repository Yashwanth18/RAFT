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
	int Send_Connection_Descriptor();
};


#endif // end of #ifndef __CLIENT_STUB_H__
