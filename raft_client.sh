#!/bin/sh
# usage: ./raft_client.sh iD (repeat iD_peer) server_role
# server role initialization: 0 = follower, 1 = candiDate, 2 = leader

# command line format:
#   ./client num_servers (repeat iD IP port_client)

#------------user's configuration before running this script---------
num_servers=1
local=1     # 0: run on VDI, 1: run on local machines
#------------End: user's configuration------------------

port_client_root=1212
IP_root="10.200.125."
IP_local="127.0.0.1"

echo Number of servers equal to $num_servers
echo ***********------------***********
echo

#----------------------Helper function------------------------#
ID_Server=()
Server_IP=()
Port_Server=()

# input: server id
Fill_Info(){      # Action: Append the three arrays above
  id=$1     # server id
  port="${port_client_root}${id}"    # port

  if [ $local -eq 0 ]; then       # ip
    ip="${IP_root}${id}"
  elif [ $local -eq 1 ]; then
    ip=$IP_local
  fi

  ID_Server+=($id)
  Port_Server+=($port)
  Server_IP+=($ip)
}

#--------------------------Main function-------------------------#
for var in "$@"
do
    Fill_Info "$var"
done


if [ $num_servers -eq 1 ]; then       # for testing purposes only
    ./client $num_servers ${ID_Server[0]} ${Server_IP[0]} ${Port_Server[0]}

elif [ $num_servers -eq 2 ]; then      # number of servers = 3

    ./client $num_servers ${ID_Server[0]} ${Server_IP[0]} ${Port_Server[0]} \
                          ${ID_Server[1]} ${Server_IP[1]} ${Port_Server[1]}

fi
