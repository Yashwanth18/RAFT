#!/bin/sh

# argv[1]: num_peers
# argv[2]: node id of self
# argv [3, 4, 5, 6]: node ids of peer server nodes
# argv[7]: port option (to deal with port in use problem): 1 or 2

if [ $1 -eq 4 ]; then
	if [ $7 -eq 1 ]; then
		echo Creating server with node id $2 with peer node ids: $3 $4 $5 $6
		./server "10100$2" $2 $1 \
							$3 "10.200.125.$3" "10100$3" \
							$4 "10.200.125.$4" "10100$4" \
							$5 "10.200.125.$5" "10100$5" \
							$6 "10.200.125.$6" "10100$6"

	elif [ $7 -eq 2 ]; then
		./server "10101$2" $2 $1 \
							$3 "10.200.125.$3" "10101$3" \
							$4 "10.200.125.$4" "10101$4" \
							$5 "10.200.125.$5" "10101$5" \
							$6 "10.200.125.$6" "10101$6"
	fi

fi
