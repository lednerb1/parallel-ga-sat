#!/bin/bash

make
if [ "$?" -ne "0" ]; then
	printf "\nFailed during Makefile, check file?\n[ENTER] | ^C\n"
	read
	vim Makefile
	clear
	printf "Quitting run.sh\n"
	exit 1
fi

mkdir outputs

declare -i proc
proc="$(nproc)"
proc=$proc-1

for t in `seq 1 $proc`
do
    printf "Setting OMP_NUM_THREADS=$t\n\n"
    export OMP_NUM_THREADS=$t
    for i in `ls -1 inputs/*.cnf | xargs -n1 basename`
    do
        for _ in `seq 1 5`
        do
            printf "Running $i\n"
            # /usr/bin/time -o outputs/$i.time ./bin/ksat.out inputs/$i
            ./bin/ksat.out inputs/$i
        done
    done
done

for o in `ls outputs`
do 
    sed -n 1p outputs/$o | awk -F"user" '{print $1}' >> outputs/runs.dat
done
