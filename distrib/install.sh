#!/bin/sh

DESTDIR=/mnt/d/minc-base
MANDIR=/usr/share/man

BIN='ksh'
LIB='libc.so.75.0 libsocket.so.75.0'
#LIB="${LIB} libcrypto.so.27.0 libutil.so.12.1"
#LIB="${LIB} libssl.so.24.1 libedit.so.5.1 libkvm.so.16.0 libsqlite3.so.27.0"
ETC='man.conf login.conf magic profile MAKEDEV console.reg syslog.conf ttys vi.exrc'
LOCALE='ARMSCII-8 ISO8859-2 KOI8-U CP1251 ISO8859-4 UTF-8 CP866 ISO8859-5 ISO8859-1 ISO8859-7 ISO8859-13
 ISO8859-9 ISO8859-15 KOI8-R'
LIBEXEC='iptables-list.sh ncurses ld.so'

function copy_dir {
	for dir in $2; do
		cp -R -p "$1/$dir" ".$1/"
	done
}
function copy_bin {
	for file in $2; do
		cp -p "$1/$file" ".$1/"
	done
}
function copy_posix {
	for file in $2; do
		cp -p "$1/$file" ".$1/"
		cp -p "$MANDIR/man1/$file.1" ".$MANDIR/man1/"
	done
}

if ! cd $DESTDIR; then
	exit 1
fi

mkdir -p ./usr/share/man/man1

mkdir -p ./bin
copy_bin /bin "$BIN"
ln ./bin/ksh ./bin/sh

cp -p "$MANDIR/man1/ksh.1" ".$MANDIR/man1/"
cp -p "$MANDIR/man1/sh.1" ".$MANDIR/man1/"

mkdir -p ./usr/lib
copy_bin /usr/lib "$LIB"

mkdir -p ./usr/libexec
copy_bin /usr/libexec "$LIBEXEC"

#mkdir -p ./usr/share/terminfo
#copy_dir /usr/share "terminfo"

mkdir -p ./etc
copy_bin /etc "$ETC"
copy_dir /etc "ssl"
copy_dir /etc "color"

#copy_dir /usr/share/locale "$LOCALE"
#copy_dir /usr/share "misc"
