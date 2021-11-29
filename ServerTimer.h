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

    /**
     * Starts the timer
     */
	void Start();

    /**
     * restarts the timer
     */
	void Restart();

    /**
     * Determines if the elections has been timed out or not
     * @return 1 if election is timed out , 0 otherwise
     */
	int Check_election_timeout();		//return 1 if election_timeout and 0 otherwise

    /**
     * prints the elapsed time
     */
	void Print_elapsed_time();

    /**
     * return max duration in millisecond for polling func
     * @return max duration in millisecond for polling func
     */
	int Poll_timeout(); 					 //return max duration in millisecond for polling func
};

#endif // end of #ifndef __Server_TIMER_H__
