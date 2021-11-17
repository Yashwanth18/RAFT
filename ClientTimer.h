#ifndef __Client_TIMER_H__
#define __Client_TIMER_H__

#include <chrono>

using namespace std::chrono;

class ClientTimer {
public:
	time_point<std::chrono::high_resolution_clock> start_time;
	duration<double, std::milli> elapsed_time;
	duration<double, std::milli> election_timeout;

	ClientTimer();

	void Start();
	void Restart();

	int Check_election_timeout();		//return 1 if election_timeout and 0 otherwise

	void Print_elapsed_time();
	int Poll_timeout(); 					 //return max duration in millisecond for polling func
};

#endif // end of #ifndef __Client_TIMER_H__
