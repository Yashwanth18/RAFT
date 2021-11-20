#include <iomanip>
#include <iostream>
#include <unistd.h>
#include "ServerTimer.h"

ServerTimer::ServerTimer() {
	srand(time(0));
    /* big randomness to decrease the probability of race condition of having multiple candidate */
	election_timeout = std::chrono::duration<double, std::milli>(150 + rand() % 1000);
}

void ServerTimer::Start() {
	sleep(2); /* allow the programmer time to run the script on multiple nodes */
	start_time = high_resolution_clock::now();
}

void ServerTimer::Restart() {
	//Print_elapsed_time();
	start_time = high_resolution_clock::now();
}

/* poll timeout small compared to election_timeout */
int ServerTimer::Poll_timeout(){
	return election_timeout.count() / 100;
}

int ServerTimer::Check_election_timeout() {
	elapsed_time = high_resolution_clock::now() - start_time;
	return (elapsed_time > election_timeout);
}

void ServerTimer::Print_elapsed_time() {
	std::cout << std::fixed << std::setprecision(3);
	std::cout << "elapsed_time: " << elapsed_time.count() << std::endl;
}
