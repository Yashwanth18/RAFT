#!/bin/sh
# argv[1]: $1 is num_peers
# argv[2]: node id of self
# argv[i+2]: $i+2 is node id of the i^{th} peer


if [ $1 -eq 0 ]; then
	echo Creating server with node id $2 and with number of peers equal to $1
	./server "10100$2" $2 $1

elif [ $1 -eq 1 ]; then
	echo Creating server with node id $2, with number of peers equal to $1 \
 			 and with	peer node ids: $3
	echo running the server program with the following arguments: \
			"10100$2" $2 $1 $3 "10.200.125.$3" "10100$3"
	./server "10100$2" $2 $1 $3 "10.200.125.$3" "10100$3"

elif [ $1 -eq 2 ]; then
	echo Creating server with node id $2 and with number of peers equal to $1 \
			 and with	peer node ids: $3 $4
	./server "10100$2" $2 $1 \
						$3 "10.200.125.$3" "10100$3" \
						$4 "10.200.125.$4" "10100$4"

elif [ $1 -eq 3 ]; then
	echo Creating server with node id $2 and with number of peers equal to $1 \
			 and with	peer node ids: $3 $4 $5
	./server "10100$2" $2 $1 \
						$3 "10.200.125.$3" "10100$3" \
						$4 "10.200.125.$4" "10100$4" \
						$5 "10.200.125.$5" "10100$5"

elif [ $1 -eq 4 ]; then
	echo Creating server with node id $2 and with number of peers equal to $1 \
			 and with	peer node ids: $3 $4 $5 $6
	./server "10100$2" $2 $1 \
						$3 "10.200.125.$3" "10100$3" \
						$4 "10.200.125.$4" "10100$4" \
						$5 "10.200.125.$5" "10100$5" \
						$6 "10.200.125.$6" "10100$6"
fi
