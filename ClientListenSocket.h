#ifndef __ClientListenSocket_H__
#define __ClientListenSocket_H__

#include <memory>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <fcntl.h>

#include "Socket.h"

class ClientListenSocket: public Socket {
public:
	ClientListenSocket() {}
	~ClientListenSocket() {}

	int Init(int port);		//return the file descriptor for listening
};


#endif // end of #ifndef __ClientListenSocket_H__
