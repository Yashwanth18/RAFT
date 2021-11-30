#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerFollowerStub.h"
#include "ServerAdminStub.h"


void Election::FollowerThread(std::unique_ptr<ServerSocket> socket) {
    ServerFollowerStub serverFollowerStub;
    serverFollowerStub.Init(std::move(socket));

    int max_data_size = sizeof(AppendEntryRequest) + sizeof(ResponseAppendEntry) +
                        sizeof(VoteRequest) + sizeof(ResponseVote);
    char buf[max_data_size];
    int messageType;

    std::cout << "max_data_size: " << max_data_size << '\n';
    socket -> Recv(buf, max_data_size, 0);
//    messageType = socket -> Unmarshal_MessageType(buf);
//
//    std::cout << "messageType: " << messageType << '\n';
}


void Election::
CandidateThread() {

}


void Election::
LeaderThread() {

}