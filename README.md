# RAFT

To Do:
* Leader Election 
> Right now, Polling is an array. To handle dynamic changes in the socket file descriptors, we need to implement Pfds as a vector. 
> Implement Order RequestVoteRPC properly. Right now, only the rough backbone logic is there. 

* Many cases to test: 
> Case where multiple nodes are candidates at the same time. 

* Log replication 
