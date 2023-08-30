#!/bin/sh

MASTER=/usr/include/sys/syscall.h

grep -ai '^#define' $MASTER | sed -e 's/SYS_//g' | \
{
	local count=0
	while read define name index; do
		echo "[$count] $name $index"
		sed "s/NAME/$name/" syscall.template >$name.S
		((count=count+1))
	done
}

