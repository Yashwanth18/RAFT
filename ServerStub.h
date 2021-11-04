#ifndef __SERVER_STUB_H__
#define __SERVER_STUB_H__

#include <memory>
#include <cstring>
#include <arpa/inet.h>
#include "ServerSocket.h"
#include "Messages.h"


class ServerStub {
private:
	std::unique_ptr<ServerSocket> socket;
	int conn_descriptor;
public:
	ServerStub();
	void Init(std::unique_ptr<ServerSocket> socket);

	//------------------------Engineer Role--------------------------------
	CustomerRequest ReceiveOrder();
	int SendLaptop(LaptopInfo info);
	int ReturnRecord(CustomerRecord record);

	//---------------------------IFA Role--------------------------------
	/* return value for the functions below:
		-1 on failure, 1 on success, and 0 if Recv return 0 (remote connection closed
	*/
	int Receive_Replication_Request(ReplicationRequest * rep_req);
	int Set_Connection_Descriptor();
	int Get_Connection_Descriptor();
	int Ship_Success_Status();

};

#endif // end of #ifndef __SERVER_STUB_H__
