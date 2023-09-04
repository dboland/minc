#!/bin/sh

MASTER=/usr/include/sys/syscall.h

echo -n "creating system call stubs..."

grep -ai '^#define' $MASTER | sed -e 's/SYS_//g' | \
{
	while read define name index; do
		if ! [ -f "$name.S" ]; then
			echo -n " $name.S"
			sed "s/NAME/$name/" syscall.template >$name.S
		fi
	done
}
echo " done."
