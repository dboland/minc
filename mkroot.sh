#!/bin/sh

DESTROOT="$1"

if [ -z "${DESTROOT}" ]; then
	echo "Usage: $0 DESTDIR"
	exit 15
fi

echo " ---------------------------------------------------"
echo -n "| Installing in ${DESTROOT} for "

if id 'NT SERVICE\TrustedInstaller' 2>&1 >/dev/null; then
	echo "Windows Vista"
	/bin/mkdir ${DESTROOT}
else
	echo "Windows NT"
	mkdir ${DESTROOT}
fi

echo " ---------------------------------------------------"
