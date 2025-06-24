# Building the system

To build the MinC Unix emulator for Windows you need MinGW, the latest 
OpenBSD source code and NSIS to create the installer. Below their 
download locations:

https://sourceforge.net/projects/mingw/
https://nsis.sourceforge.io/Download

I pulled the OpenBSD source code from the read-only GitHub repository. 
There is an article which explains how to pull the source for a specific 
stable version, but you need the CVS software:

https://www.openbsd.org/anoncvs.html

## Stage 1: build the kernel

For this you need the OpenBSD 6.1.0 source code. You can get it yourself 
using cvs, but you can also use my copy (186Mb). Move this file to 
your 'minc-devel' directory and extract it there:

https://minc.commandlinerevolution.nl/source/openbsd-master-6.1.0.zip

Most of the kernel can be built using a vanilla MinGW installation. Open 
the Msys terminal, cd to the 'minc-devel' directory and make the 
kernel first:

	make kernel

When the first stage was succesful, you will get the following output:

	fatal error: machine/cdefs.h: No such file or directory

This means we are ready for Stage 2.

## Stage 2: build a minimal build system

Next, you will need to unmount the /mingw directory and mount the directory 
for your new system. This is be done by running the mount.sh script:

	./mount.sh minc

To finish the build:

	make all
