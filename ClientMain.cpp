#include "ClientMain.h"

#include "ClientTimer.h"
#include <thread>

int main(int argc, char *argv[]) {
    std::vector<Peer_Info> PeerServerInfo;
    ClientTimer timer;
    std::vector<std::shared_ptr<ClientThreadClass>> client_vector;
    std::vector<std::thread> thread_vector;
    int num_customers;
    int num_orders;
    int request_type;

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo))    { return 0; }

    // to-do: accept from command line
    num_customers = 1;
    num_orders = 1;
    request_type = LEADER_ID_REQUEST;
    //request_type = READ_REQUEST;

    std::string ip = PeerServerInfo.at(0).IP;
    int port = PeerServerInfo.at(0).port;

    timer.Start();
    for (int i = 0; i < num_customers; i++) {
        auto client_cls = std::shared_ptr<ClientThreadClass>(new ClientThreadClass());
        std::thread client_thread(&ClientThreadClass::ThreadBody, client_cls,
                                  ip, port, i, num_orders, request_type);

        client_vector.push_back(std::move(client_cls));
        thread_vector.push_back(std::move(client_thread));
    }

    for (auto& th : thread_vector) {
        th.join();
    }
    timer.End();

    for (auto& cls : client_vector) {
        timer.Merge(cls->GetTimer());
    }
    // timer.PrintStats();

}
