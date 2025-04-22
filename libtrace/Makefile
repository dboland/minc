include ../Makefile.inc

RELEASE=1.0.0
VERSION=20200126
LIB=libtrace.a

all: ${LIB}

%.d: %
	${MAKE} -C $<

${LIB}: win.d vfs.d
	ar cru $@ $(shell /bin/ls -f win/*.o vfs/*.o)

clean:
	${MAKE} -C win clean
	${MAKE} -C vfs clean
