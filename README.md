# RAFT

To Do:
* Leader Election 
> Handle error for send and recv. Right now, if a follower node quit, everything crashes. 
> Implement Order RequestVoteRPC properly. Right now, only the rough backbone logic is there. 

* Many cases to test: 
> Case where multiple nodes are candidates at the same time. 
> Case where leader fails
> Case where follower fails
* Log replication 
> For ease of debuggin and development purpose, assume the client program takes the role of a follower node. 

Documentation for all the functions are present in the header files and not in cpp files.
