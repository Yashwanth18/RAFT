#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "ServerSocket.h"
#include "ServerThread.h"

int main(int argc, char *argv[]) {
	int port;
	int engineer_cnt = 0;
	int num_peers;
	ServerSocket socket;
	LaptopFactory factory;
	std::unique_ptr<ServerSocket> new_socket;
	std::vector<std::thread> thread_vector;

	if (argc < 4) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] <<
		"[port #] [# unique ID] [# peers] (repeat [ID] [IP] [port #])" << std::endl;
		return 0;
	}

	port = atoi(argv[1]);
	factory.SetFactoryId(atoi(argv[2]));
	num_peers = atoi(argv[3]);
	factory.SetNumPeers(num_peers);

	if (factory.FillPeerServerInfo(argc, argv) < 0)		return 0;

	std::thread admin_thread(&LaptopFactory::AdminThread, &factory);
	thread_vector.push_back(std::move(admin_thread));

	if (!socket.Init(port)) {
		std::cout << "Socket initialization failed" << std::endl;
		return 0;
	}

	while ((new_socket = socket.Accept())) {
		std::thread engineer_thread(&LaptopFactory::EngineerThread,
				&factory, std::move(new_socket),
				engineer_cnt++);

		thread_vector.push_back(std::move(engineer_thread));
	}
	return 0;
}
