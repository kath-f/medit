#!/usr/bin/bash

while [ 0 ] 
do
	if [ -d "./build/" ]; then
		cmake -S . -B build/
		make -C build/

		break
	else
		mkdir build/
		continue
	fi
done
