# Building the system

To build the MinC Unix emulator for Windows you need MinGW, 
the latest OpenBSD source code and NSIS to create the installer. 
Below their download locations:

* MinGW32: https://sourceforge.net/projects/mingw/
* NSIS: https://nsis.sourceforge.io/Download

I pulled the **OpenBSD** source code from the read-only GitHub 
repository. There is an article which explains how to pull the 
source for a specific stable version, but you need 
the CVS software:

https://www.openbsd.org/anoncvs.html

## Stage 1: build the kernel

For this you need the OpenBSD 6.1.0 source code. As mentined 
above, you can get it yourself using cvs, but you can also use 
my copy (186Mb). Move this file to preferably the root of a disk 
drive and extract it there:

https://minc.commandlinerevolution.nl/openbsd-master-6.1.0.zip

Most of the kernel can be built using a vanilla MinGW installation. 
Open the Msys terminal, cd to the 'minc-devel' directory and make 
the kernel first:

	make kernel

When the first stage was succesful, you will get the following 
output. This means we are ready for Stage 2:

	fatal error: machine/cdefs.h: No such file or directory

## Stage 2: build a minimal system

A minimal system consists of the kernel, the BSD libc library, the 
boot program, the shell and some utilities. These will be built by 
the new system itself. To achieve this, you will need to unmount 
the /mingw directory. This is done by running the mount.sh script:

	./mount.sh minc

Now you can finish the build:

	make all

You now have a working OpenBSD system. To test this, you can run 
the **uname** command:

	uname -a

It should be similar to the following output:

	OpenBSD dimension.sassenheim.dmz 6.1.0 MINC#20240915 i386
