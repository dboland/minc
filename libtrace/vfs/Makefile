include ../../Makefile.inc

OBJECTS = VfsFileAttribs.o VfsFileFlags.o VfsPathFlags.o VfsVolumeFlags.o \
	VfsTaskFlags.o VfsInputMode.o VfsNetEvents.o VfsScreenMode.o
OBJECTS += vfs_CAP_CONTROL.o vfs_VNODE.o vfs_NAMEI.o vfs_DEVICE.o vfs_MOUNT.o \
	vfs_STATFS.o vfs_INODE.o vfs_TASK.o vfs_THREAD.o vfs_TTY.o vfs_CONIN.o \
	vfs_ACCESS.o vfs_NETFLAGS.o vfs_CFDATA.o

CFLAGS += -DWIN_ERR=printf

all: ${OBJECTS}

%.o: %.c
	gcc -c ${CFLAGS} -I${MINGW}/include -I../../include -I.. -o $@ $<

clean:
	rm -f *.o ${LIB}
