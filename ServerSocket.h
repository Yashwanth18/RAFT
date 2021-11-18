#pragma once
#ifndef __SERVERSOCKET_H__
#define __SERVERSOCKET_H__
#include <string>
#include "Socket.h"
#include "Messages.h"
#include "fcntl.h"

struct SocketInfo{
    int fd;
    int socket_status;
};
class ServerSocket: public Socket {
public:
	ServerSocket() {}
	~ServerSocket() {}
    SocketInfo* Init(std::string ip, int port);
};
#endif