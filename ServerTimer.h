#ifndef __Server_TIMER_H__
#define __Server_TIMER_H__

#include <chrono>

using namespace std::chrono;

class ServerTimer {
private:
    std::mutex lock_timer;
    time_point<std::chrono::high_resolution_clock> start_time;
    duration<double, std::milli> elapsed_time;
    duration<double, std::milli> election_timeout;
    duration<double, std::milli> waitRequest_timeout;

public:
	ServerTimer();
	void Start();
	void Atomic_Restart();              // restart with mutex protection

	bool Check_Election_timeout();		// return 1 if Election_timeout and 0 otherwise
    bool WaitRequest_timeout();
	void Print_elapsed_time();
};

#endif // end of #ifndef __Server_TIMER_H__
