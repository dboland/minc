#!/bin/sh

DESTROOT="$1"

if [ -z "${DESTROOT}" ]; then
	echo "Usage: $0 DESTDIR"
	exit 15
fi

echo " ---------------------------------------------------"
echo -n "| Installing in ${DESTROOT} for "

if id 'NT SERVICE\TrustedInstaller' >/dev/null 2>&1; then
	echo "Windows Vista"
	/bin/mkdir -p ${DESTROOT}
else
	echo "Windows NT"
	mkdir -p ${DESTROOT}
fi

echo " ---------------------------------------------------"
