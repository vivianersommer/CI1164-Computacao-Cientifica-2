#!/bin/bash

for N in 200
#10 32 50 64 100 128 200 250 256 300 400 512 600 1000 1024 2000 2048 3000 4096
do
	echo $N
 	echo "(3-2*x1)*x1-2*x2+1"
	for i in $(seq 2 $((N-1)))
	do
		echo "(3-2*x$((i)))*x$((i))-x$((i-1))-2*x$((i+1))+1"
	done
 	echo "(3-2*x$((N)))*x$((N))-x$((N-1))+1"
 	for i in $(seq 1 $N)
	do
		echo -n "-1 "
	done
	if [ $N -eq 128 ]
	then
 		echo -e "\n0\n20"
	else
		echo -e "\n0\n20\n"
	fi
done
