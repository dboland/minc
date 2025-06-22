#!/bin/sh

ROOT=/mnt/d/libminc-0.6.1
#RELEASE=/mnt/d/minc-base
PKGROOT=/mnt/d/src

DISTROOT=${ROOT}/distrib
MINIROOT=${DISTROOT}/miniroot
LATEST=/mnt/d/commandlinerevolution.nl/minc/release

SBIN='libposix-6.1.0.dll bsd.exe mkent wrc setup.sh wtrace iptables color terminal.cmd mount_refs'
ETC='man.conf login.conf magic profile MAKEDEV console.reg syslog.conf ttys vi.exrc'

copy_file()
{
	for file in $2; do
		cp -p "$1/$file" ".$1/$file"
	done
}

if ! cd "$MINIROOT"; then
	exit 1
fi

cp /bin/tar tar.exe
cp /bin/chmod chmod.exe
cp /bin/sh sh.exe
cp /sbin/libposix-6.1.0.dll .
cp ${ROOT}/mingw/bin/scacls.exe .
cp /mnt/d/src/gzip-1.13/gzip.exe .

. $DISTROOT/test.sh
#. $DISTROOT/comp.sh
. $DISTROOT/base.sh

#. $DISTROOT/vista.sh
#. $DISTROOT/vim.sh
#. $DISTROOT/nano.sh
#. $DISTROOT/lynx.sh
#. $DISTROOT/wget.sh
#. $DISTROOT/curl.sh
#. $DISTROOT/unzip.sh
#. $DISTROOT/bzip2.sh
#. $DISTROOT/email.sh
#. $DISTROOT/hexedit.sh
#. $DISTROOT/xz.sh
#. $DISTROOT/mutt.sh
#. $DISTROOT/bitchx.sh
#. $DISTROOT/git.sh
#. $DISTROOT/magick.sh
#. $DISTROOT/rsync.sh
#. $DISTROOT/binutils.sh
#. $DISTROOT/gcc.sh
#. $DISTROOT/gmake.sh
#. $DISTROOT/perl.sh
#. $DISTROOT/bind.sh
#. $DISTROOT/sasl.sh

wrc makensis "d:/libminc-0.6.1/distrib/minc.nsi"
#wrc makensis "d:/libminc-0.6.1/distrib/buildtools.nsi"
wrc makensis "d:/libminc-0.6.1/distrib/test.nsi"
#git archive --format=zip --prefix=minc-master/ -o release/source/minc-master.zip HEAD
