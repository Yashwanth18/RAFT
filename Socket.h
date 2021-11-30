#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <sys/poll.h>
#include <sys/select.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <cstring>
#include <iostream>

#define NAGLE_ON	0
#define NAGLE_OFF	1

class Socket {
protected:
	bool is_initialized_;
    int fd_;

private:
	int nagle_;
public:

	Socket();
	virtual ~Socket();

	int Send(char *buffer, int size, int flags = 0);
	int Recv(char *buffer, int size, int flags = 0);

    int Unmarshal_MessageType(char *buf); // return the messageType

	int NagleOn(bool on_off);
	bool IsNagleOn();

	void Close();
};


#endif // end of #ifndef __SOCKET_H__
