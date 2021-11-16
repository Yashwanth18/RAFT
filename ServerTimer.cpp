#include <iomanip>
#include <iostream>
#include <unistd.h>
#include "ServerTimer.h"

ServerTimer::ServerTimer() {
	srand(time(0));

	//election_timeout random between 1 to 2 millisecond
	election_timeout = std::chrono::duration<double, std::micro>(1000 + rand() % 1000);
}

void ServerTimer::Start() {
	//allow the programmer time to run the script on multiple nodes
	sleep(3 + rand() % 3);

	start_time = high_resolution_clock::now();
}

void ServerTimer::Restart() {
	start_time = high_resolution_clock::now();
}

//poll timeout small compared to election_timeout
int ServerTimer::Poll_timeout(){
	return election_timeout.count() / 10000;
}

int ServerTimer::Check_election_timeout() {
	auto end_time = high_resolution_clock::now();
	elapsed_time = (end_time - start_time);

	if (elapsed_time > election_timeout){
		return 1;
	}
	else{
		return 0;
	}
}

void ServerTimer::Print_elapsed_time() {
	std::cout << std::fixed << std::setprecision(3);
	std::cout << "elapsed_time: " << elapsed_time.count() << "\t" << std::endl;
}

void ServerTimer::Print_election_timeout() {
	std::cout << std::fixed << std::setprecision(3);
	std::cout << "elapsed_time: " << elapsed_time.count() << "\t" << std::endl;
}
