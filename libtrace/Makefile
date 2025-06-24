include ../Makefile.inc

all: win.d vfs.d libtrace.a

%.d: %
	@${MAKE} -C $<

libtrace.a:
	ar cru $@ $(shell /bin/ls -f win/*.o vfs/*.o)

clean:
	/bin/rm -f win/*.o vfs/*.o
