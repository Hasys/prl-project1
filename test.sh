#!/bin/bash
counter=0
COUNT="$1"
cores=$(($COUNT + 1))
declare -r HEX_DIGITS="0123456789ABCDEF"

if [ $# -eq 1 ]; then
	while [ $counter -lt "$1" ]
	do
		# získame randomně čislo
		dec_value=$(((RANDOM % 255) + 1))
		hex_value=""

		# převod s DEC do HEX
		until [ $dec_value == 0 ]; do
			rem_value=$((dec_value % 16))
			dec_value=$((dec_value / 16))
			hex_digit=${HEX_DIGITS:$rem_value:1}
			hex_value="${hex_digit}${hex_value}"
		done

		# pišeme náhodná čisla do souboru
		echo -n -e "\\x${hex_value}" >> ./numbers

		counter=$(($counter + 1))
	done
else
	echo "Must be only 1 parametr - count of numbers to create"
	exit 0
fi

# překlad s zdrojaku
mpicc --prefix /usr/local/share/OpenMPI -o es es.c
# spusteni
mpirun --prefix /usr/local/share/OpenMPI -np $(($cores)) es 

# úklid
rm -f es
rm -f ./numbers