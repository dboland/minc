# Building the system

To build the MinC Unix emulator for Windows you need the 
OpenBSD 6.1.0 source code and MinGW:

* https://sourceforge.net/projects/mingw/

I pulled the OpenBSD source code from the read-only GitHub 
repository. There is an article which explains how to pull 
the source for a specific version, but you will need the 
CVS software:

* https://www.openbsd.org/anoncvs.html

However, you can also use my copy (186Mb):

* https://minc.commandlinerevolution.nl/openbsd-master-6.1.0.zip

**Note**: create a folder in the root of a diskdrive, named 
'source'. Move the .ZIP file there and extract it.

## Step 0: set up a simple cross-compiler

To build the new kernel and compile the operating system code, 
you would have to build an OpenBSD cross compiler first. This 
can be very laborious and frustrating. If you want to know more 
about cross-compiling, watch Rob Landley's excellent presentation 
on the topic:

* https://youtu.be/Sk9TatW9ino

Instead we will create an isolated, slightly modified instance 
of the MinGW compiler. Let's call it a *poor man's cross-compiler*. 
Your current MinGW installation will be left completely 
untouched.

Open the MSYS terminal, **cd** to your *minc-devel* directory 
and make the *opt* target. All files will be installed in a 
newly created directory, called */opt*:

	make opt

**Note**: you will probably get some error messages here. These 
will guide you to properly set up the 'config.inc' file. Use 
vim to edit it and define the indicated variables.

## Step 1: build the kernel

The kernel will be built using a combination of vanilla MinGW 
and our *poor man's cross-compiler*, residing in */opt/cross*. 
To test if this all works, make the kernel first:

	make kernel

## Step 2: build a minimal system

A minimal system consists of the kernel, the BSD C library, the 
boot program, the Korn shell and some utilities. These will be 
built by the new system itself. To finish the build:

	make system

You now have a working OpenBSD system, but we are still using 
the MSYS commands. To make sure we are executing OpenBSD commands, 
you will need to unmount the */mingw* directory. This is done by 
running the **mount** script:

	./mount.sh minc

To test if the new system works, you can run the **uname** command:

	uname -a

The result should be similar to the following output:

	OpenBSD dimension.sassenheim.dmz 6.1.0 MINC#20250720 i386

**Note**: if you want back to compiling Windows programs, simply 
re-mount the */mingw* directory:

	./mount.sh mingw

## Step 3: install the system

To install your new system you have to manually create the root 
directory, run the **install** target and the **setup** program, 
thereby mimicking what is done automatically by the MinC installer.
This is to avoid accidentally writing into an existing MinC system. 

For this step, you need to be Administrator. Close the MSYS 
terminal and open it again as Administrator. Change to your 
*minc-devel* directory and create the MinC root directory. Name 
it *minc-release* so it won't conflict with an existing MinC 
system:

	install -o root -g wheel -d /c/minc-release

This command will create the MinC root directory (**-d**) with 
*root* as the owner (**-o**) and *wheel* as group (**-g**). 
MinC automatically translates these names to their Windows 
equivalents (*SYSTEM* and *Administrators*).

**Note**: it is not advisable to create the MinC root directory 
in a location like *Program Files*. Remember, we will be building 
an entire operating system. File permissions in this kind of 
location are unsuitable for OpenBSD to run properly.

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

Double-clicking the *bsd.exe* program is not the right way to 
start the MinC terminal. To make it start properly and in your 
home directory, put following lines in a file named 
*minc-release.cmd* on your Desktop:

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
the MinC root folder in Windows Explorer, just exit the console 
or type its location in the address bar, like *C:\minc-release*.

## Step 4: building the GCC compiler

This is currently not within the scope of this document.

Daniel Boland, July, 2025
