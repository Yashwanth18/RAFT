#include <string>
#include "Socket.h"
#include "Messages.h"




class ServerAdminSocket: public Socket {
public:
	ServerAdminSocket() {}
	~ServerAdminSocket() {}

	int Init(std::string ip, int port);
};
