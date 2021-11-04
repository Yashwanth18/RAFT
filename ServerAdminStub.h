#include <string>
#include "ServerAdminSocket.h"
#include "Messages.h"

class ServerAdminStub {
private:
	ServerAdminSocket socket;
public:
	ServerAdminStub();

	/* return value for the functions below:
		-1 on failure, 1 on success, and 0 if Recv return 0 (remote connection closed
	*/

	int Init(std::string ip, int port);
	int Send_Connection_Description();
  int Order_Replication_Request(ReplicationRequest * rep_req);
};
