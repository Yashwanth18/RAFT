#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerStub.h"
#include "ServerAdminStub.h"


void Election::
FollowerThread(std::unique_ptr<ServerSocket> socket, int id) {
    std::cout << "Nonsense: " << id << '\n';
}

