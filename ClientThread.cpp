#include <iostream>
#include "ClientThread.h"
#include "Messages.h"

ClientThreadClass::ClientThreadClass() {}

void ClientThreadClass::
ThreadBody(std::string ip, int port, int id, int orders, int type) {
	customer_id = id;
	num_orders = orders;
	request_type = type;
	int status = 1;

	status = stub.Init(ip, port);

	if (status == 1){
		status = stub.Send_Connection_Descriptor();
	}
	else{
		return;
	}
	if (request_type == 1){

		for (int i = 0; i < num_orders; i++) {

			if(status == 1){
				CustomerRequest order;
				LaptopInfo laptop;
				order.SetOrder(customer_id, i, request_type);

				timer.Start();
				status = stub.OrderLaptop(order, &laptop);
				//laptop.Print();
				timer.EndAndMerge();

				//only print invalid laptop if order was successful
				if (status == 1){
					if (!laptop.IsValid()) {
						std::cout << "Invalid laptop " << customer_id << std::endl;
						break;
					}
				}

			}

			else{
				break;
			}

		}
	}
	else if (request_type == 2){
		CustomerRequest request;
		CustomerRecord record;
		request.SetOrder(customer_id, -1, request_type);

		status = stub.ReadRecord(request, &record);
	}

	else if (request_type == 3){
		CustomerRequest request;
		CustomerRecord record;
		request_type = 2;

		for (int i = 0; i < num_orders; i++){
			if (status == 1){
				request.SetOrder(i, -1, request_type);
				status = stub.ReadRecord(request, &record);
				if (record.IsValid()){	//if the record exists, print
					record.Print();
				}
			}
			else{
				break;
			}
		}
	}
}

ClientTimer ClientThreadClass::GetTimer() {
	return timer;
}
