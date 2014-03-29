#!/bin/bash
myvar=0
COUNT="$1"
cores=$(($COUNT + 1))

if [ $# -eq 1 ]; then
	while [ $myvar -lt "$1" ]
	do
		echo $(((RANDOM % 255) + 1 )) >> ./numbers
		myvar=$(($myvar + 1))
	done
else
	echo "Must be only 1 parametr - count of numbers to create"
	exit 0
fi

mpicc --prefix /usr/local/share/OpenMPI -o es es.c	# preklad cpp zdrojaku
mpirun --prefix /usr/local/share/OpenMPI -np $(($cores)) es 	#spusteni

#uklid
rm -f es
rm -f ./numbers