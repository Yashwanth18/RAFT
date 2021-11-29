#include "ServerSocket.h"



void ServerSocket::AddSocketToPoll(int new_fd, std::vector<pollfd> *_pfds_server) {
    pollfd new_pfd;
    new_pfd.fd = new_fd;
    new_pfd.events = POLLIN;
    _pfds_server -> push_back(new_pfd);
}

int ServerSocket::Unmarshal_MessageType(char *buf){
    int net_messageType;
    int messageType;

    memcpy(&net_messageType, buf, sizeof(net_messageType));
    messageType = ntohl(net_messageType);
    return messageType;
}

void ServerSocket::Accept_Connection(std::vector<pollfd> *_pfds_server){
    int new_fd;
    struct sockaddr_in addr;
    unsigned int addr_size = sizeof(addr);

    // the listening socket is pfds_server[0].fd
    new_fd = accept((*_pfds_server)[0].fd, (struct sockaddr *) &addr, &addr_size);
    if (new_fd < 0) perror ("accept");

    AddSocketToPoll(new_fd, _pfds_server);
}

int ServerSocket::Send_Message(char *buf, int size, int fd){
    int remain_size = size;
    int offset = 0;
    int bytes_written;

    while (remain_size > 0){
        try{
            bytes_written = send(fd, buf+offset, remain_size, 0);
            if (bytes_written < 0){
                throw bytes_written;
            }
        }
        catch(int stat){
            return 0;
        }
        offset += bytes_written;
        remain_size -= bytes_written;
    }
    return 1;
}

/* return 1 if success and 0 if failure */
int ServerSocket::Read_Message(int fd, char *buf, int size){
    int bytes_read = 0;
    int offset = 0;
    while (size > 0) {
        try{
            bytes_read = recv(fd, buf + offset, size, 0);
            if (bytes_read <= 0) {
                throw bytes_read;
            }
        }
        catch (int stat){
            return 0;
        }

        assert(bytes_read != 0);

        size -= bytes_read;
        offset += bytes_read;
        assert(size >= 0);
    }

    return 1;

}

/* return 0 on failure and 1 on success */
int ServerSocket::Connect_To(std::string ip, int port, int new_fd){
    struct sockaddr_in addr;
    int connect_status;

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);


    try{
        connect_status = connect(new_fd, (struct sockaddr *) &addr, sizeof(addr));
        if (connect_status < 0){
            throw connect_status;
        }
    }
    catch(int stat){
        return 0;
    }

    fcntl(new_fd, F_SETFL, O_NONBLOCK);   /* set socket to non-blocking after connection successful*/
    return 1;
}

int ServerSocket::Create_Socket() {
    int new_fd;
    new_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (new_fd < 0) {
        perror("ERROR: failed to create a socket");
        return -1;
    }

    return new_fd;
}