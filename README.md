# RAFT

### Design

- ### Leader Election:
    - #### Log structure:
        - Log entry has a term ( the term in which log entry is created), and a command associated with it (user input)
    - #### Candidate Perspective:
        - When the system starts, all the servers will be in the follower state as their default state, and when one of the servers gets timed out, it becomes a candidate, an election for a leader starts.
        - To avoid the conflict of multiple followers becoming a candidate at the same time, we use randomized timeout values for different servers.
        - The Candidate will then start sending Request Vote RPC, which contains its ID, term, last log index, and last log term to all the peers in the network.
        - Candidate will continue sending Request Vote RPC to the followers until:
            1. It becomes a leader or
            2. If any other server becomes a leader (in this case, the current candidate would translate to the followers state)
            3. A period of time goes by with no winner, in this case election term increases to start a new election
        - Once the candidate receives the majority of votes in the current term, it will then become a leader
    - #### Follower Perspective:
        - Upon receiving the vote request from the candidate, follower would compare its last log term, last log index, with the candidates last log term and last log index, and would grant vote to the candidate if:
            1. Candidates term is greater than the followers term, and
            2. If the candidates log is at least as up-to-date as receiver’s log
            3. And if the follower hasn’t voted for any other candidate in the current term of the election
- ### Log Replication:
    - #### Leader Perspective:
        - Once a leader has been elected in an election, the leader is now ready to accept the client requests to replicate the request among the followers
        - Leader receives the request from a client, sends append entry rpc to the followers
        - Once the leader's commit index matches up with index of write request, leader then responds back to the client, i.e., if the log entry has been replicated at majority of servers
        - The arguments for append entry rpc are:
            1. term : leader’s term
            2. leaderId : so follower can redirect clients
            3. prevLogIndex : index of log entry immediately preceding new ones
            4. prevLogTerm : term of prevLogIndex entry
            5. Entry : log entry to store
            6. leaderCommit : leader’s commitIndex
    - #### Follower Perspective:
        - Follower does the following to determine if it can accept the append entry request coming from a leader:
            1. Reply false if term < currentTerm
            2. Reply false if log doesn’t contain an entry at prevLogIndex whose term matches prevLogTerm
            3. If an existing entry conflicts with a new one (same index but different terms), delete the existing entry and all that follow it
            4. Append any new entries not already in the log
            5. If leaderCommit > commitIndex, set commitIndex = min(leaderCommit, index of last new entry)
        - RAFT Guarantees the following invariant:
            1. If log entries on different servers have same index and term, then they store the same command, and the logs are identical in all preceding entries
        - MatchIndex: for each server, index of highest log entry known to be replicated on server
        - Updating the commit index of the leader:
            1. If there exists an N such that N greater than commitIndex, a majority of matchIndex[i] greater than or equal to N, and log[N].term == currentTerm:
            2. set commitIndex = N
        - Due to the leader crash scenarios, it might happen that, there might be log inconsistencies, and to correct this issue, leader maintains a nextIndex variable for each server, which indicates the nextIndex that leader will send to that follower
        - In case of log inconsistencies, leader would decrease the nextIndex value and retry append entry rpc until append entry rpc gets succeeded.
        - Whenever follower overwrites inconsistency entry, it deletes all the subsequent entries
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

### Instructions to run the program:
- ### Command line arguments to run on vdi machines:
    - We have written 2 shell scripts - one to run the client and other to run the server
    - Server:
        1. run the script with title - "Raft-server.sh" as:
        2. ./Raft-server.sh [server ID] [repeat([peer ID])] [server role]
        3. example: ./Raft-server.sh 36 37 38 39 40 1, this examples refers to a 5 server network, where 36 is the node id of the server, and 37, 38, 39, 40 are the node ids of the peers in the network. 1 is the server role,
        4. server role parameter accepts three types of roles: 0 - Follower, 1 - Candidate and 2- Leader
    - Client:
        1. run the script with title - "raft-client.sh" as:
        2. ./raft-client.sh [repeat(server ID)] [number of customers] [number of orders for each customer] [request type]
        3. example: According to the quoted example above to run the server, we have 5 servers running, whose IDs are 36, 37, 38, 39, 40, so the command line arguments for client would be:
        4. ./raft-client.sh 36 37 38 39 40 4 100 2, here 4 represents number of customers, 100 represents number of orders for each customer, and 2 represents the request type
        5. requests type can be 1 - Write Request, 2 - Read Request, 4 - Fetch Leader ID Request
    - Since all the vdi machines belongs to a cluster, all the machines have the ip address ending with "10.200.125.", the last 2 digits represents the id of a machine, and we have used that to represent server id as the command line argument to run the script
    - The scripts support running of 3 server network and 5 server, and you can change the configuration by changing the num of servers variable in the script
    - To run the program on a local machine, local variable has to be set to 1, and to run on the vdi cluster, it has to be set to 0
