# RAFT

* Command line format: ./server port_server port_client nodeID num_peers (repeat peerID IP port_server)
* Server role initialization: 0 = follower, 1 = candidate, 2 = leader

* Usage of the script ***local_server.sh***: ./local_server.sh nodeID (repeat ID_peer) server_role 
* For example, the code below would create a server node ID 21, initialized to be a follower, 
with 4 peers 22, 23, 24, 25.
> ./local_server.sh 21 22 23 24 25 0 
