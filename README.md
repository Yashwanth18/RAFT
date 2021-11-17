# RAFT

To Do:
* Leader Election 
> Handle error for send and recv. Right now, if a follower node quit, everything crashes. 
> Implement Order RequestVoteRPC properly. Right now, only the rough backbone logic is there. 

* Many cases to test: 
> Case where multiple nodes are candidates at the same time. 

* Log replication 
> For ease of debuggin and development purpose, create an optional command line argument that initialize the node to be leader or follower. Then, implement log replication in the case of no failure. 
