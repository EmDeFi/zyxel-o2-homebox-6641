#!/bin/bash


list=$(find . -name "*svn")

for var in $list
do
	if [ -e ${var} ]; then
		rm -rf ${var}
		echo "${var} is removed!"
	fi
done
