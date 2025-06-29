include ../../../Makefile.inc

OBJECTS= adler32.o compress.o crc32.o deflate.o gzio.o infback.o \
	inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o
HEADERS= zlib.h zconf.h

all: ${HEADERS} libz.a

%.o: ${SRCDIR}/lib/libz/%.c
	@/bin/echo -n "$@ "
	@gcc -c ${CFLAGS} -o $@ $<

%.h: ${SRCDIR}/lib/libz/%.h
	@/bin/cp -vu $< ${INCDIR}/

libz.a: ${OBJECTS}
	@/bin/echo "done."
	@/bin/echo "Making static library $@"
	@ar cru $@ ${OBJECTS}

install-local:
	/bin/cp -vu libz.a ${LIBDIR}/
