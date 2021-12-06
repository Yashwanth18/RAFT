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
    std::mutex print_lck;

    if (!FillPeerServerInfo(argc, argv, &PeerServerInfo, &PeerIdIndexMap))    { return 0; }



    if (argc < 4){
        std::cout << " Invalid command line format " << '\n';
        return 0;
    }

    num_customers = atoi(argv[argc - 3]);
    num_orders = atoi(argv[argc - 2]);
    request_type = atoi(argv[argc - 1]);    // last input from command line

    std::cout << "num_customers:  " << num_customers << '\n';
    std::cout << "num_orders:  " << num_orders << '\n';
    std::cout << "request_type:  " << request_type << '\n' << '\n';

    timer.Start();
    for (int i = 0; i < num_customers; i++) {
        auto client_cls = std::shared_ptr<ClientThreadClass>(new ClientThreadClass());
        std::thread client_thread(&ClientThreadClass::ThreadBody, client_cls,
                                  &PeerServerInfo, &PeerIdIndexMap, i, num_orders,
                                  request_type, &print_lck);

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

    if (request_type == WRITE_REQUEST){
        timer.PrintStats();
    }

}
