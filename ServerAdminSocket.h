#include <string>
#include "Socket.h"
#include "Messages.h"




class ServerAdminSocket: public Socket {
public:
	ServerAdminSocket() {}
	~ServerAdminSocket() {}

	bool Init(std::string ip, int port);
};
