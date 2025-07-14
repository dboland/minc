# Building the system

To build the MinC Unix emulator for Windows you need the 
OpenBSD 6.1.0 source code and MinGW:

* https://sourceforge.net/projects/mingw/

I pulled the OpenBSD source code from the read-only GitHub 
repository. There is an article which explains how to pull 
the source for a specific version, but you will need the 
CVS software:

* https://www.openbsd.org/anoncvs.html

However, you can also use my copy (186Mb). Move this file to 
preferably the root of a disk drive and extract it there:

* https://minc.commandlinerevolution.nl/openbsd-master-6.1.0.zip

To build the new kernel and compile the operating system code, 
we would have to install the OpenBSD header files into the 
*/include* directory, build some libraries and put them in 
the */lib* directory. This would make MinGW unusable for normal 
operation, so we create an isolated instance of the compiler. 
Let's call it a *poor man's cross-compiler*. 

Open the MSYS terminal, **cd** to your *minc-devel* directory 
and make the *opt* target. All files will be installed in a 
newly created directory, called */opt*:

	make opt

## Step 1: build the kernel

Most of the kernel will be built using the vanilla MinGW 
installation. Make this part of the kernel first:

	make kernel

When the first step was successful, you will get the following 
output. This means we are ready for step 2:

	fatal error: machine/_types.h: No such file or directory

## Step 2: build a minimal system

A minimal system consists of the kernel, the BSD C library, the 
boot program, the shell and some utilities. These will be built by 
the new system itself. To achieve this, you will need to unmount 
the */mingw* directory. This is done by running the **mount** script:

	./mount.sh minc

To finish the build:

	make all

You now have a working OpenBSD system. To test this, you can run 
the **uname** command:

	uname -a

The result should be similar to the following output:

	OpenBSD dimension.sassenheim.dmz 6.1.0 MINC#20240915 i386

**Note**: if you want back to building Windows programs, simply 
re-mount the */mingw* directory:

	./mount.sh mingw

## Step 3: install the system

To install your new system you have to manually create the root 
directory, run the **install** target and the **setup** program, 
thereby mimicking what is done automatically by the MinC installer.
This is to avoid accidentally writing into an existing MinC system. 

For this step, you need to be Administrator. Close the MSYS 
terminal and open it again as Administrator. Change to your 
*minc-devel* directory and create the MinC root folder. I 
named it *minc-release* so it won't conflict with an existing 
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
along with some of the other system directories like */home* and 
*/tmp*, run the **setup** script:

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
the Windows Registry. The *-h* switch tells MinC to change to 
your */home* directory.

**Note**: the root folder you created seems to have disappeared.
This is because when MinC mounts its root directory, it hides the 
directory from view in Windows Explorer. This is super important 
for preventing recursive directory listings. It is also standard 
practice in Windows for mounted drives. If you want to access 
the MinC root folder in Windows Explorer, just type its location 
in the address bar, like *C:\minc-release* and you're back.

## Step 4: building the GCC compiler

This is currently not within the scope of this document.

Daniel Boland, July, 2025
