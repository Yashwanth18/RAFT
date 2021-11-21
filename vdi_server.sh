#!/bin/sh
# usage: ./local_server.sh ID num_peers (repeat ID_peer)
# command line format: ./server port_server port_client ID num_peers (repeat ID IP port_server)

port_client=101080
port_server_root=2020
node_ID=$1


#if [ $2 -eq 1 ]; then
#	if [ $7 -eq 1 ]; then
#		echo Creating server with node id $2 with peer node ids: $3 $4 $5 $6
#		./server "10100$2" $2 $1 \
#							$3 "10.200.125.$3" "10100$3" \
#							$4 "10.200.125.$4" "10100$4" \
#							$5 "10.200.125.$5" "10100$5" \
#							$6 "10.200.125.$6" "10100$6"
#
#	elif [ $7 -eq 2 ]; then
#		./server "10101$2" $2 $1 \
#							$3 "10.200.125.$3" "10101$3" \
#							$4 "10.200.125.$4" "10101$4" \
#							$5 "10.200.125.$5" "10101$5" \
#							$6 "10.200.125.$6" "10101$6"
#	fi
#
#fi
