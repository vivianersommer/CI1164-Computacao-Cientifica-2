#!/bin/bash

mkdir "sl"

for N in 10 32 50 64 100 128
# 200 250 256 300 400 512 600 1000 1024 2000 2048 3000 4096
do
	echo $N > "sl/sl_$N.dat"
 	echo "(3-2*x1)*x1-2*x2+1" >> "sl/sl_$N.dat"
	for i in $(seq 2 $((N-1)))
	do
		echo "(3-2*x$((i)))*x$((i))-x$((i-1))-2*x$((i+1))+1" >> "sl/sl_$N.dat"
	done
 	echo "(3-2*x$((N)))*x$((N))-x$((N-1))+1" >> "sl/sl_$N.dat"
 	for i in $(seq 1 $N)
	do
		echo -n "-1 " >> "sl/sl_$N.dat"
	done
 	echo -e "\n0\n20" >> "sl/sl_$N.dat"
done
