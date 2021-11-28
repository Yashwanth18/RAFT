#ifndef __SERVERSOCKET_H__
#define __SERVERSOCKET_H__

#include <poll.h>
#include <vector>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <map>

#include "ServerListenSocket.h"
#include "ServerTimer.h"
#include "Messages.h"


class ServerSocket{
public:
    ServerSocket() {};

    /* Accept Connection and Recv*/
    void Accept_Connection(std::vector<pollfd> *_pfds_server);
    void Socket_Add_Socket_To_Poll(int new_fd, std::vector<pollfd> *_pfds_server);

    /* Connect */
    int Create_Socket();
    int Connect_To(std::string ip, int port, int fd);

    /* Send */
    int Send_Message(char *buf, int size, int fd);

};



#endif // #ifndef __SERVERSOCKET_H__