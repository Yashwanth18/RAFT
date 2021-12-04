#include "ClientMain.h"
#include "ClientTimer.h"
#include <thread>

int main(int argc, char *argv[]) {
    std::vector<Peer_Info> PeerServerInfo;
    std::map<int,int> PeerIdIndexMap;
    ClientTimer timer;
    std::vector<std::shared_ptr<ClientThreadClass>> client_vector;
    std::vector<std::thread> thread_vector;
    int num_customers;
    int num_orders;
    int request_type;

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo, &PeerIdIndexMap))    { return 0; }

    request_type = atoi(argv[argc - 1]);    // last input from command line

    /* configure these two variables for evaluation purposes  */
    num_customers = 1;
    num_orders = 1;

    timer.Start();
    for (int i = 0; i < num_customers; i++) {
        auto client_cls = std::shared_ptr<ClientThreadClass>(new ClientThreadClass());
        std::thread client_thread(&ClientThreadClass::ThreadBody, client_cls,
                                  &PeerServerInfo, &PeerIdIndexMap, i, num_orders, request_type);

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
