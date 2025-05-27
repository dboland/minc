include ../Makefile.inc

all: libtrace.a

%.d: %
	@${MAKE} -C $<

libtrace.a: win.d vfs.d
	ar cru $@ $(shell /bin/ls -f win/*.o vfs/*.o)

clean:
	${MAKE} -C win clean
	${MAKE} -C vfs clean
