#include <iomanip>
#include <iostream>
#include <unistd.h>
#include "ClientTimer.h"

ClientTimer::ClientTimer() {
	srand(time(0));


	election_timeout = std::chrono::duration<double, std::milli>(150 + rand() % 1000);
}

void ClientTimer::Start() {
	//allow the programmer time to run the script on multiple nodes
	sleep(2);
	start_time = high_resolution_clock::now();
}

void ClientTimer::Restart() {
	//Print_elapsed_time();
	start_time = high_resolution_clock::now();
}

//poll timeout small compared to election_timeout
int ClientTimer::Poll_timeout(){
	return election_timeout.count() / 10;
}

int ClientTimer::Check_election_timeout() {
	elapsed_time = high_resolution_clock::now() - start_time;
	return (elapsed_time > election_timeout);
}

void ClientTimer::Print_elapsed_time() {
	std::cout << std::fixed << std::setprecision(3);
	std::cout << "elapsed_time: " << elapsed_time.count() << std::endl;
}
