#include <string>
#include "Socket.h"
#include "Messages.h"




class ServerOutSocket: public Socket {
public:
	ServerOutSocket() {}
	~ServerOutSocket() {}

	bool Init(std::string ip, int port);
};
