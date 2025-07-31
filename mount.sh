#!/bin/sh

ROOTDIR=$(dirname $0)

. ${ROOTDIR}/config.inc

# Hello Earnie Boyd #

MINGW=$(cmd //c echo ${MINGW})
MINCROOT=$(cmd //c echo ${PREFIX})
FSTAB=/etc/fstab

if ! [ -d ${MINGW} ]; then
	echo "${MINGW}: No such directory"
elif [ "$1" == "mingw" ]; then
	echo "${MINGW} /mingw" >$FSTAB
elif ! [ -d ${MINCROOT} ]; then
	echo "${MINCROOT}: No such directory"
elif [ "$1" == "minc" ]; then
	echo "${MINCROOT} /mingw" >$FSTAB
else
	echo "Usage $0 COMPILER"
	echo
	echo "Compilers"
	printf " mingw\t\t\tmount MinGW default compiler\n"
	printf " minc\t\t\tmount OpenBSD cross-compiler\n"
	echo
	echo "Currently mounted:"
	/bin/cat $FSTAB
fi
