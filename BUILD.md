# Building the system

To build the MinC Unix emulator for Windows you need MinGW, 
the latest OpenBSD source code and NSIS to create the installer. 
Below their download locations:

* MinGW32: https://sourceforge.net/projects/mingw/
* NSIS: https://nsis.sourceforge.io/Download

I pulled the *OpenBSD* source code from the read-only GitHub 
repository. There is an article which explains how to pull the 
source for a specific stable version, but you need 
the CVS software:

https://www.openbsd.org/anoncvs.html

**Warning**: if you go forward, be advised that the MSYS system 
running MinGW will be modified to function as an OpenBSD system.
Your MinGW installation will be left completely untouched, but 
the MSYS system running it, will not. If you already use MinGW 
and expect it to continue creating Windows programs, you 
should **not** go ahead and build the MinC Unix emulator.

## Step 1: build the kernel

For this you need the OpenBSD 6.1.0 source code. As mentioned, 
you can get it yourself using cvs, but you can also use my 
copy (186Mb). Move this file to preferably the root of a disk 
drive and extract it there:

https://minc.commandlinerevolution.nl/openbsd-master-6.1.0.zip

Most of the kernel can be built using a vanilla MinGW installation. 
Open the MSYS terminal, **cd** to the *minc-devel* directory and make 
the kernel first:

	make kernel

When the first stage was successful, you will get the following 
output. This means we are ready for step 2:

	fatal error: machine/_types.h: No such file or directory

## Step 2: build a minimal system

A minimal system consists of the kernel, the BSD libc library, the 
boot program, the shell and some utilities. These will be built by 
the new system itself. To achieve this, you will need to unmount 
the /mingw directory. This is done by running the **mount** script:

	./mount.sh minc

To finish the build:

	make all

You now have a working OpenBSD system. To test this, you can run 
the **uname** command:

	uname -a

It should be similar to the following output:

	OpenBSD dimension.sassenheim.dmz 6.1.0 MINC#20240915 i386

## Step 3: install the system

To install the new system you have to manually create the root 
directory, run the **install** target and the **setup** program, 
thereby mimicking what is done automatically by the MinC installer.
This is also to avoid accidentally writing into an existing 
MinC system. 

For this step, you need to be Administrator. Close the MSYS 
terminal and open it again as Administrator. Change to your 
*minc-devel* directory and create the MinC root folder. I 
name it *minc-release* so it won't conflict with an existing 
MinC system:

	mkdir /c/minc-release

Now you can run the **install** target:

	make install

To finalize step 3, go to the new folder in Windows Exporer and 
open the *sbin* folder. There should be a program named *bsd.exe*.
Right-click it and run it as Administrator. You will see the 
following message:

	open(/dev/console): No such file or directory
	/bin/ksh: No controlling tty (open /dev/tty: No such file or directory)
	/bin/ksh: warning: won't have full job control

This is because there is no */dev* file system yet. To create it,
along with some of the other system directories like 'home' and 
'tmp', run the **setup** script:

	/sbin/setup.sh

When it is finished, you can close the window and open it again 
by double-clicking. The above message should be gone.

Double-clicking the *bsd.exe* program is not the right way to start
the MinC terminal. To make it start properly and in your home directory,
put following lines in a file named *minc-release.cmd* on your Desktop:

	@ECHO OFF
	
	CD C:\minc-release\sbin
	START "MinC" bsd.exe -h

The *START* command tells the console to read console settings from
the Registry. The *-h* switch tells MinC to change to your *home* 
directory.

A final note: the root folder you created seems to have disappeared.
This is because when MinC mounts its root directory, it hides the 
directory from view in Windows Explorer. This is super important 
for preventing recursive directory listings. It is also standard 
practice in Windows for mounted drives.

If you want to access the MinC root folder in Windows Explorer, just
type its location in the address bar, like *C:\minc-release* and 
you're back.

## Step 4: building the GCC compiler

This is currently not within the scope of this document.

Daniel Boland, June, 2025
