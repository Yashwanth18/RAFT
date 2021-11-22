#!/bin/sh

# usage: ./node_server.sh ID (repeat ID_peer) server_role

# command line format:
#   ./server port_server port_client ID num_peers (repeat ID Ip port_server) server_role
# server role initialization: 0 = follower, 1 = candidate, 2 = leader

#------------user's configuration  before run---------
num_peers=1
#------------End: user's configuration------------------

port_server_root=2020
port_client=101080
run_location=0
Ip_root="10.200.125."


Set_Peer_Ip(){
    if [ $run_location -eq 0 ]; then    # run on VDI cluster
      Ip_Peer1="127.0.0.1"
      echo Running on local machines

    elif [ $run_location -eq 1 ]; then   # run on local machines
        Ip_root="10.200.125."
        Ip_Peer1=${Ip_root}${node_ID}
        echo Running on VDI machines

    else
      echo Undefined run_location
    fi
}

# parameter for the function Print_Program_Info():
  # argv[1] = node_ID
  # argv[2] = ID_Peer1
  # argv[3] = server_role

Print_Program_Info(){
  if [ $3 -eq 0 ]; then
    echo Creating a follower server with ID: $1 and ID_peers: $2

  elif [ $3 -eq 1  ]; then
    echo Creating a candidate server with ID: $1 and ID_peers: $2

  elif [ $3 -eq 2 ]; then
    echo Creating a leader server with ID: $1 and ID_peers: $2

  else
    echo Undefined server role initialization
  fi
}

###-------------- main function--------------------###
node_ID=$1
port_server=${port_server_root}${node_ID}

if [ $num_peers -eq 1 ]; then
  ID_Peer1=$2
  server_role=$3
  Set_Peer_Ip
  Print_Program_Info $node_ID $ID_Peer1 $server_role

  ./server "${port_server_root}${node_ID}" $port_client $node_ID $num_peers \
              $ID_Peer1 $Ip_Peer1 "${port_server_root}${ID_Peer1}" \
              $server_role
fi

