#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerStub.h"
#include "ServerAdminStub.h"

LaptopFactory::LaptopFactory(){
	last_index = -1;
	committed_index = -1;
	primary_id = -1;
	num_peers = -1;
	factory_id = -1;
}

void LaptopFactory::SetFactoryId(int id)		{	factory_id = id; }
void LaptopFactory::SetNumPeers(int num)		{num_peers = num; }

/*
Takes in the command line argument and fill in PeerServerInfo
return -1 if command line format error
*/
int LaptopFactory:: FillPeerServerInfo(int argc, char *argv[]){
	for (int i = 1; i <= num_peers; i++){
		if (argc <= 3*i + 3){
			std::cout << "not enough arguments" << std::endl;
			std::cout <<
			"./server [port #] [unique ID] [# peers] (repeat [ID] [IP] [port #])	"
			<< std::endl;
			return -1;
		}
		else{
			int unique_id = atoi(argv[3*i + 1]);
			std::string IP = argv[3*i + 2];
			int port = atoi(argv[3*i + 3]);

			ServerInfo peer_server_info {unique_id, IP, port};
			PeerServerInfo.push_back(peer_server_info);
		}
	}

	return 0;
}

LaptopInfo LaptopFactory::
CreateLaptop(CustomerRequest request, int engineer_id) {
	LaptopInfo laptop;
	laptop.CopyOrder(request);
	laptop.SetEngineerId(engineer_id);

	std::promise<LaptopInfo> prom;
	std::future<LaptopInfo> fut = prom.get_future();

	std::unique_ptr<AdminRequest> req =
		std::unique_ptr<AdminRequest>(new AdminRequest);
	req->laptop = laptop;
	req->prom = std::move(prom);

	erq_lock.lock();
	erq.push(std::move(req));
	erq_cv.notify_one();
	erq_lock.unlock();

	laptop = fut.get();
	return laptop;
}

/*
takes a replication request, push it to smr_log, and
execute the last committed operation
*/
void LaptopFactory::Handle_Replication_Request(ReplicationRequest * rep_req){
	int opcode;
	int arg1;
	int arg2;

	primary_id = rep_req -> Get_factory_id();
	last_index = rep_req->Get_last_index();
	committed_index = rep_req->Get_committed_index();

	opcode = rep_req->Get_opcode();
	arg1 = rep_req->Get_arg1();
	arg2 = rep_req->Get_arg2();

	MapOp map_op_new {opcode, arg1, arg2};

	smr_log.push_back(map_op_new);
	Apply_Committed_Op();
}

/*
get the MapOp from smr_log at committed_index, and then
execute the operation in the Map <int, int>
*/
void LaptopFactory::Apply_Committed_Op(){
	MapOp mapOp_commited = smr_log[committed_index];
	if (mapOp_commited.opcode == 1){
		MapRecord_lock.lock();
		MapCustomerRecord [mapOp_commited.arg1] = mapOp_commited.arg2;
		MapRecord_lock.unlock();
	}
}

void LaptopFactory::
EngineerThread(std::unique_ptr<ServerSocket> socket, int id) {
	int connection_descriptor;
	ServerStub stub;

	stub.Init(std::move(socket));

	stub.Set_Connection_Descriptor();
	connection_descriptor = stub.Get_Connection_Descriptor();


	if (connection_descriptor == CUST_CONNECTED){
		CustomerRequest request;
		CustomerRecord record;
		LaptopInfo laptop;
		int engineer_id = id;
		int request_type;

		while (true) {
			request = stub.ReceiveOrder();

			if (!request.IsValid()) {
				break;
			}
			request_type = request.GetRequestType();

			switch (request_type) {
				case 1:
					laptop = CreateLaptop(request, engineer_id);
					stub.SendLaptop(laptop);
					break;

				case 2:
					Fill_Customer_Record(&request, &record);
					stub.ReturnRecord(record);
					break;

				default:
					std::cout << "Undefined request_type: "
						<< request_type << std::endl;
			}
		}
	}

	else if (connection_descriptor == PFA_CONNECTED){
		ReplicationRequest rep_req;
		int conn_status = 1;

		while(true){
			if (conn_status == 1){
				conn_status = stub.Receive_Replication_Request(&rep_req);
			}
			else {
				primary_id = -1;
				break;
			}

			if (conn_status == 1){
				Handle_Replication_Request(&rep_req);
				conn_status = stub.Ship_Success_Status();
			}
			else {
				primary_id = -1;
				break;
			}
		}

	}
	else{
		std::cout << "undefined connection descriptor" << '\n';
	}
}

/*
Takes a customer request, find it in the Map <int, int>, and fill the record
*/
void LaptopFactory::
Fill_Customer_Record(CustomerRequest * request, CustomerRecord * record){
	std::map<int,int>::iterator iter;

	MapRecord_lock.lock();		//lock Map
	iter = MapCustomerRecord.find(request -> GetCustomerId());

	if (iter == MapCustomerRecord.end()){				//record does not exist in map
		record -> SetCustomerId(-1);
		record -> SetLastOrder(-1);
	}
	else{
		record -> SetCustomerId(request -> GetCustomerId());
		record -> SetLastOrder(MapCustomerRecord[ request -> GetCustomerId() ]);
	}

	MapRecord_lock.unlock();	//unlock Map
}

void LaptopFactory::Init_Socket_Status(int * status){
	for (int i = 0; i < num_peers; i++){
		status[i] = 1;
	}
}

void LaptopFactory::AdminThread() {
	std::unique_lock<std::mutex> ul(erq_lock, std::defer_lock);
	ServerAdminStub admin_stub[num_peers];
	int status [num_peers];
	int opcode = 1;
	int arg1;
	int arg2;

	Init_Socket_Status(status);

	while (true) {
		//wait for request to arrive from the regular engineer in the queue
		ul.lock();
		if (erq.empty()) {
			erq_cv.wait(ul, [this]{ return !erq.empty(); });
		}

		auto req = std::move(erq.front());
		erq.pop();
		ul.unlock();

		primary_id = factory_id;

		//if switch from idle to production factory
		if (committed_index == last_index + 1){
			committed_index++;
			Apply_Committed_Op();
		}

		//update smr_log
		arg1 = req->laptop.GetCustomerId();
		arg2 = req->laptop.GetOrderNumber();
		MapOp map_op {opcode, arg1, arg2};
		smr_log.push_back(map_op);

		last_index++;

		//prepare replication request
		ReplicationRequest rep_req;
		rep_req.Set_Rep_Req(factory_id, committed_index, last_index, opcode, arg1, arg2);

		//replication with failure handling
		for (int i = 0; i < num_peers; i++){
			if (status[i] == 1){
				status[i] = admin_stub[i].Init(PeerServerInfo[i].IP, PeerServerInfo[i].port);
				//std::cout << "status[i] after init: " << status[i] << '\n';

				if (status[i] == 1){
					status[i] = admin_stub[i].Send_Connection_Description();
					//std::cout << "status[i] after Send_Connection_Description: " << status[i] << '\n';
				}

				if (status[i] >= 0){
					status[i] = admin_stub[i].Order_Replication_Request(&rep_req);
					//std::cout << "status[i] after order replication request: " << status[i] << '\n';
				}
			}
		}


		committed_index = last_index;
		Apply_Committed_Op();

		req->laptop.SetAdminId(primary_id);
		req->prom.set_value(req->laptop);
	}
}
