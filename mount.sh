#!/bin/sh

. config.inc

MINGW=$(echo -n ${MINGW} | sed -E 's/^\/(.)/\1\:/')
MINC=${MINGW}/msys/1.0${PREFIX}
FSTAB=/etc/fstab

if ! [ -d ${MINGW} ]; then
	echo "${MINGW}: No such directory"
elif [ "$1" == "mingw" ]; then
	echo "${MINGW} /mingw" >$FSTAB
elif ! [ -d ${MINC} ]; then
	echo "${MINC}: No such directory"
elif [ "$1" == "minc" ]; then
	echo "${MINC} /mingw" >$FSTAB
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
