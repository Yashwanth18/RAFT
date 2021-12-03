#include <iostream>
#include <memory>

#include "Server_ClientThread.h"

void Interface::Listening_Client(ServerSocket *clientSocket) {
    while (true) {
        std::unique_ptr<ServerSocket> new_socket;
        new_socket = clientSocket -> Accept();
        std::cout << "Accepted Connection from client" << '\n';
    }
}

