#!/bin/sh


# argv[5]: port option (to deal with port in use problem): 1 or 2

if [ $1 -eq 2 ]; then
	if [ $5 -eq 1 ]; then
		echo Creating server with node id $2 with peer node ids: $3 $4
		./server "10100$2" $2 $1 \
							$3 "10.200.125.$3" "10100$3" \
							$4 "10.200.125.$4" "10100$4"

	elif [ $5 -eq 2 ]; then
		./server "10101$2" $2 $1 \
							$3 "10.200.125.$3" "10101$3" \
							$4 "10.200.125.$4" "10101$4"
	fi

fi
