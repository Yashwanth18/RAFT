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

    /** Accepts the socket connection and adds it to the vector of pfds_server
     *
     * @param _pfds_server a vector of pollfd from where it reads the data
     */
    void Accept_Connection(std::vector<pollfd> *_pfds_server);

    /**
     * Adds a new pfd to the vector of pfds
     * @param new_fd the file descriptor of the new connection
     * @param _pfds_server a vector of pollfd from where it reads the data
     */
    void Socket_Add_Socket_To_Poll(int new_fd, std::vector<pollfd> *_pfds_server);

    /* Connect */
    /**
     * It creates a new socket to connect to a server
     * @return a file descriptor if a socket gets created successfully, if not -1
     */
    int Create_Socket();

    /**
     * Connects to a specific server by taking its ip address and the port
     * @param ip ip address of the server
     * @param port port number of the server
     * @param fd file descriptor of a socket
     * @return 1 if it successfully connects, 0 otherwise
     */
    int Connect_To(std::string ip, int port, int fd);

    /* Send */
    /**
     * Sends a message across the network
     * @param buf buffer containing the data
     * @param size of the the data
     * @param fd the file descriptor of the socket to whom data is being sent
     * @return 1 if data is successfully sent, 0 otherwise
     */
    int Send_Message(char *buf, int size, int fd);

};



#endif // #ifndef __SERVERSOCKET_H__