#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerFollowerStub.h"
#include "ServerOutStub.h"


void Election::
FollowerThread(std::unique_ptr<ServerSocket> socket, NodeInfo *nodeInfo, ServerState *serverState) {

    // ServerFollowerStub serverFollowerStub;
    // serverFollowerStub.Init(std::move(socket));

    int max_data_size = sizeof(AppendEntryRequest) + sizeof(ResponseAppendEntry) +
                        sizeof(VoteRequest) + sizeof(ResponseVote);
    char buf[max_data_size];
    int read_status = socket -> Recv(buf, sizeof(VoteRequest), 0);

    std::cout<< "read_status: " << read_status << '\n';

    if (read_status > 0){
      int messageType;
      int net_messageType;
      memcpy(&net_messageType, buf, sizeof(net_messageType));
      messageType = ntohl(net_messageType);


      if (messageType == VOTE_REQUEST){ // main functionality
        VoteRequest voteRequest;
        voteRequest.Unmarshal(buf);
        voteRequest.Print();
      }
      char buf[1];
      int ack = htonl(messageType);
      memcpy(buf, &ack, sizeof(ack));
      socket -> Send(buf, 1, 0);
    }




}


void Election::
CandidateThread(int peer_index, std::vector<Peer_Info> *PeerServerInfo,
                NodeInfo *nodeInfo, ServerState *serverState) {
}


void Election::
LeaderThread() {

}
