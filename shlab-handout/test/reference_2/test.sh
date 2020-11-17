#!/bin/bash

for file in $(ls trace*)
do
	./sdriver.pl -t $file -s ./tshref > tshref_$file
	./sdriver.pl -t $file -s ./tsh > tsh_$file
done 

for file in $(ls trace*)
do
	diff tsh_$file tshref_$file > diff_$file
done

for file in $(ls diff_trace*)
do
	echo $file " :"
	cat $file
	echo -e "-------------------------------------\n"
