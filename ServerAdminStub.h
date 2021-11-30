#include <string>
#include "ServerAdminSocket.h"
#include "Messages.h"

class ServerAdminStub {
private:
	ServerAdminSocket socket;
public:
	ServerAdminStub();

	int Init(std::string ip, int port);
};
