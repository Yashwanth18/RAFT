#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>

#define PFA_CONNECTED 1
#define CUST_CONNECTED 2

struct MapOp{
	int opcode;
	int arg1;
	int arg2;
};

/*------------------------ReplicationRequest Class--------------------*/
class ReplicationRequest{
private:
	int factory_id;
	int committed_index;
	int last_index;

	//map_op
	int opcode;
	int arg1;
	int arg2;

public:
	ReplicationRequest();
	void Set_Rep_Req(int id_factory, int index_commit, int index_last,
									int opcode, int arg1, int arg2);

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	int Get_factory_id()					{return factory_id;}
	int Get_committed_index() 		{return committed_index;}
	int Get_last_index()					{return last_index;}
	int Get_opcode()							{return opcode;}
	int Get_arg1() 								{return arg1;}
	int Get_arg2()								{return arg2;}

	int Size();

};

/*-------------------------Class: CustomerRecord-------------------------------*/
class CustomerRecord {
private:
	int customer_id; 	//copied from the read request
										//-1 if customer_id is not found in the map

	int last_order; 	//copied from the map
										//-1 if customer_id is not found in the map
public:
	CustomerRecord();
	void operator = (const CustomerRecord &record) {
		customer_id = record.customer_id;
		last_order = record.last_order;
	}

	void SetCustomerId(int customer_id);
	void SetLastOrder(int last_order);

	int GetCustomerId();
	int GetLastOrder();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	int Size();
	bool IsValid();
	void Print();
};


/*---------------------------Class: CustomerRequest---------------------------*/
class CustomerRequest {
private:
	int customer_id;
	int order_number;
	int request_type;

public:
	CustomerRequest();
	void operator = (const CustomerRequest &order) {
		customer_id = order.customer_id;
		order_number = order.order_number;
		request_type = order.request_type;
	}
	void SetOrder(int cid, int order_num, int type);
	int GetCustomerId();
	int GetOrderNumber();
	int GetRequestType();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();
	int Size();
	void Print();
};
/*---------------------------Class: LaptopInfo---------------------------------*/
class LaptopInfo {
private:
	int customer_id;
	int order_number;
	int request_type;
	int engineer_id;
	int admin_id;

public:
	LaptopInfo();
	void operator = (const LaptopInfo &info) {
		customer_id = info.customer_id;
		order_number = info.order_number;
		request_type = info.request_type;
		engineer_id = info.engineer_id;
		admin_id = info.admin_id;
	}
	void SetInfo(int cid, int order_num, int type, int engid, int expid);
	void CopyOrder(CustomerRequest order);
	void SetEngineerId(int id);
	void SetAdminId(int id);

	int GetCustomerId();
	int GetOrderNumber();
	int GetRequestType();
	int GetEngineerId();
	int GetAdminId();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	int Size();
	bool IsValid();
	void Print();
};

#endif // #ifndef __MESSAGES_H__
