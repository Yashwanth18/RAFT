#include <iomanip>
#include <iostream>
#include <unistd.h>
#include "ServerTimer.h"

ServerTimer::ServerTimer() {
	srand(time(0));
    election_timeout = std::chrono::duration<double, std::milli>(1000 + rand() % 1000);
    waitRequest_timeout = election_timeout / 4;
}


void ServerTimer::Start() {
    std::unique_lock<std::mutex> ul_timer(lock_timer, std::defer_lock);
    ul_timer.lock();
	start_time = high_resolution_clock::now();
}

void ServerTimer::Atomic_Restart() {
    std::unique_lock<std::mutex> ul_timer(lock_timer, std::defer_lock);
    ul_timer.lock();

	start_time = high_resolution_clock::now();
}


bool ServerTimer::Check_Election_timeout() {
    std::unique_lock<std::mutex> ul_timer(lock_timer, std::defer_lock);
    ul_timer.lock();

	elapsed_time = high_resolution_clock::now() - start_time;
	return (elapsed_time > election_timeout);
}

bool  ServerTimer::WaitRequest_timeout() {
    std::unique_lock<std::mutex> ul_timer(lock_timer, std::defer_lock);
    ul_timer.lock();
    elapsed_time = high_resolution_clock::now() - start_time;
    return (elapsed_time > waitRequest_timeout);
}

void ServerTimer::Print_elapsed_time() {
    std::unique_lock<std::mutex> ul_timer(lock_timer, std::defer_lock);
    ul_timer.lock();

    elapsed_time = high_resolution_clock::now() - start_time;
	std::cout << std::fixed << std::setprecision(3);
	std::cout << "elapsed_time: " << elapsed_time.count() << std::endl;
}
