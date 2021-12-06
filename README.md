# RAFT

### Instructions to run the program:
We have written 2 shell scripts - one to run the client and other to run the server. To run the program on a local machine, local variable has to be set to 1, and to run on the vdi cluster, it has to be set to 0. The scripts support running of 3 server network and 5 server, and you can change the configuration by changing the num of servers variable in the script.

## Server:
Since all the vdi machines belongs to a cluster, all the machines have the ip address ending with "10.200.125.", the last 2 digits represents the id of a machine, and we have used that to represent server id as the command line argument to run the script. 
  * run the script with title - "Raft_server.sh" as:  ./Raft_server.sh [server ID] [repeat([peer ID])] [server role]
 
* ./Raft-server.sh 36 37 38 39 40 1, 
> this command refers to a 5 server network, where 36 is the node id of the server, and 37, 38, 39, 40 are the node ids of the peers in the network. Server role parameter accepts three types of roles: 0 - Follower, 1 - Candidate and 2- Leader
 

## Client:
        1. run the script with title - "raft_client.sh" as:
        2. ./raft_client.sh [repeat(server ID)] [number of customers] [number of orders for each customer] [request type]
        3. example: According to the quoted example above to run the server, we have 5 servers running, whose IDs are 36, 37, 38, 39, 40, so the command line arguments for client would be:
        4. ./raft-client.sh 36 37 38 39 40 4 100 2, here 4 represents number of customers, 100 represents number of orders for each customer, and 2 represents the request type
        5. requests type can be 1 - Write Request, 2 - Read Request, 4 - Fetch Leader ID Request
   
   
  



- ### Client Interaction:
    - Client provides three features, write request, read request, and a request to get the leader id.
    - Write request:
        1. Client makes a request to a random server, and it keeps connecting until it finds a leader
        2.  Once client connects to the client, it sends the info about number of customers, and the number of orders each customer would like to place
        3. Once the leader has successfully replicated the write request among its peers, it successfully returns to the client
        4.  Mean Write Latency, and Throughput is computed at the clients end
    - Read request:
        1.  Client makes a read request to any random server in the network
        2. If the random server is a follower, then the client would get the latest entry in that follower, it might not be the latest entry as that of the leader
        3. The server response will contain each customers last order id
    - Leader ID request:
        1. Client will make a leader ID request to the random server, and in return will get the leader ID


