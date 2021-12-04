#ifndef __CLIENT_THREAD_H__
#define __CLIENT_THREAD_H__

#include <chrono>
#include <ctime>
#include <string>
#include <map>
#include "ClientStub.h"
#include "ClientTimer.h"
#include "Messages.h"

class ClientThreadClass {
    int customer_id;
    int num_orders;
    int request_type;
    ClientStub stub;
    ClientTimer timer;
public:
	ClientThreadClass();
	void ThreadBody(std::vector<Peer_Info> *PeerServerInfo,
                    std::map<int,int> *PeerIdIndexMap,
                    int id, int orders, int type);
    ClientTimer GetTimer();

    bool Connect_ServerIndex(std::vector<Peer_Info> *PeerServerInfo,
                             int index);
    bool Connect_Leader(std::vector<Peer_Info> *PeerServerInfo,
                        std::map<int,int> *PeerIdIndexMap);
    bool Connect_ServerRandomIndex(std::vector<Peer_Info> *PeerServerInfo,
                                   int *serverIndex);

};


#endif // end of #ifndef __CLIENT_THREAD_H__
