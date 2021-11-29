#!/bin/sh
# usage: ./local_server.sh ID (repeat ID_peer) server_role
# server role initialization: 0 = follower, 1 = candidate, 2 = leader

# command line format: ./server port_server port_client ID num_peers (repeat ID Ip port_server)

#------------user's configuration before running this script---------
num_peers=2
local=0     # 0: run on VDI, 1: run on local machines
#------------End: user's configuration------------------


port_server_root=2021
port_client=101080
run_location=0
IP_root="10.200.125."
IP_local="127.0.0.1"

Print_init_server_role(){
  if [ $server_role -eq 0 ]; then
    echo Initialize server role to be a Follower
  elif [ $server_role -eq 1 ]; then
    echo Initialize server role to be a Candidate
  elif [ $server_role -eq 2 ]; then
    echo Initialize server role to be a Leader
  else
    echo Undefined server role initialization
  fi
}

###------------------------------ main function-------------------------###
node_ID=$1
port_server=${port_server_root}${node_ID}


if [ $num_peers -eq 2 ]; then      # number of servers = 3
    ID_Peer1=$2
    ID_Peer2=$3
    server_role=$4

    if [ $local -eq 0 ]; then
      ip_peer1="${IP_root}${ID_Peer1}"
      ip_peer2="${IP_root}${ID_Peer2}"
    elif [ $local -eq 1 ]; then
      ip_peer1=IP_local
      ip_peer2=IP_local
    fi

    echo $ip_peer1
    echo $ip_peer2

    echo Number of peer servers equal to $num_peers
    echo ***********------------*********************

    ./server "${port_server_root}${node_ID}" $port_client $node_ID $num_peers \
              $ID_Peer1 $ip_peer1 "${port_server_root}${ID_Peer1}" \
              $ID_Peer2 $ip_peer2 "${port_server_root}${ID_Peer2}" \
              $server_role

# elif [ $num_peers -eq 4 ]; then       # number of servers = 5
#     ID_Peer1=$2
#     ID_Peer2=$3
#     ID_Peer3=$4
#     ID_Peer4=$5
#     server_role=$6
#
#     echo Number of peer servers equal to $num_peers
#     echo ***********------------*********************
#
#     ./server "${port_server_root}${node_ID}" $port_client $node_ID $num_peers \
#               $ID_Peer1 $IP_local "${port_server_root}${ID_Peer1}" \
#               $ID_Peer2 $IP_local "${port_server_root}${ID_Peer2}" \
#               $ID_Peer3 $IP_local "${port_server_root}${ID_Peer3}" \
#               $ID_Peer4 $IP_local "${port_server_root}${ID_Peer4}" \
#               $server_role
#
#
# else
#     echo undefined option for number of peer servers
fi
