#!/bin/sh
# usage: ./raft_client.sh iD (repeat iD_peer) requestType
# requestType --
  # 1: write request
  # 2: read your own request
  # 3: read all customers records
  # 4: ask for leader ID


# command line format:
#   ./client num_servers (repeat iD IP port_client) requestType

#------------user's configuration before running this script---------
num_servers=5
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



if [ $num_servers -eq 1 ]; then       # number of servers = 1


    num_customers=$2
    num_order=$3
    requestType=$4

    ./client $num_servers ${ID_Server[0]} ${Server_IP[0]} ${Port_Server[0]} \
                          "$num_customers" "$num_order" "$requestType"


elif [ $num_servers -eq 3 ]; then      # number of servers = 3

    num_customers=$4
    num_order=$5
    requestType=$6

    ./client $num_servers ${ID_Server[0]} ${Server_IP[0]} ${Port_Server[0]} \
                          ${ID_Server[1]} ${Server_IP[1]} ${Port_Server[1]} \
                          ${ID_Server[2]} ${Server_IP[2]} ${Port_Server[2]} \
                          "$num_customers" "$num_order" "$requestType"


elif [ $num_servers -eq 5 ]; then      # number of servers = 5


    num_customers=$6
    num_order=$7
    requestType=$8

    ./client $num_servers ${ID_Server[0]} ${Server_IP[0]} ${Port_Server[0]} \
                          ${ID_Server[1]} ${Server_IP[1]} ${Port_Server[1]} \
                          ${ID_Server[2]} ${Server_IP[2]} ${Port_Server[2]} \
                          ${ID_Server[3]} ${Server_IP[3]} ${Port_Server[3]} \
                          ${ID_Server[4]} ${Server_IP[4]} ${Port_Server[4]} \
                          "$num_customers" "$num_order" "$requestType"

fi
