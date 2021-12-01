#ifndef __Server_TIMER_H__
#define __Server_TIMER_H__

#include <chrono>

using namespace std::chrono;

class ServerTimer {
public:
	time_point<std::chrono::high_resolution_clock> start_time;
	duration<double, std::milli> elapsed_time;
	duration<double, std::milli> election_timeout;

	ServerTimer();

	void Start();
	void Restart();

	int Check_Election_timeout();		//return 1 if Election_timeout and 0 otherwise

	void Print_elapsed_time();
};

#endif // end of #ifndef __Server_TIMER_H__
