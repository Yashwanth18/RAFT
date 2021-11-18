#pragma once
#ifndef __SERVERSOCKET_H__
#define __SERVERSOCKET_H__
#include <string>
#include "Socket.h"
#include "Messages.h"


struct socketInfo{
    int fd;
    int socketStatus;
};
class ServerSocket: public Socket {
public:
	ServerSocket() {}
	~ServerSocket() {}
    socketInfo* Init(std::string ip, int port);
};
#endif