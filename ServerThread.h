#ifndef __SERVERTHREAD_H__
#define __SERVERTHREAD_H__

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <map>

#include "Messages.h"
#include "ServerSocket.h"

struct AdminRequest {
	LaptopInfo laptop;
	std::promise<LaptopInfo> prom;
};

struct ServerInfo{
	int unique_id;
	std::string IP;
	int port;
};

class LaptopFactory {
private:
	int num_peers;
	int factory_id;
	int primary_id;
	int last_index;
	int committed_index;

	std::mutex MapRecord_lock;
	std::map<int, int> MapCustomerRecord;
	std::vector<MapOp> smr_log;

	std::vector<ServerInfo> PeerServerInfo;

	std::queue<std::unique_ptr<AdminRequest>> erq;
	std::mutex erq_lock;
	std::condition_variable erq_cv;

	LaptopInfo CreateLaptop(CustomerRequest order, int engineer_id);
public:
	LaptopFactory();

	void EngineerThread(std::unique_ptr<ServerSocket> socket, int id);
	void AdminThread();

	int FillPeerServerInfo(int argc, char* argv[]);
	void Fill_Customer_Record(CustomerRequest * request, CustomerRecord * record);

	void SetNumPeers(int num);
	void SetFactoryId(int id);
	void Init_Socket_Status(int * status);

	void Handle_Replication_Request(ReplicationRequest * rep_req);
	void Apply_Committed_Op();


};
#endif // end of #ifndef __SERVERTHREAD_H__
