.SUFFIXES:
.DEFAULT:

all: string.d gen.d net.d sys.d

%.d: %
	@${MAKE} -C $< CFLAGS="${CFLAGS}" SRCDIR=${SRCDIR} ARCH=${ARCH}

%.m: %
	@${MAKE} -C $< install SRCDIR=${SRCDIR} DESTDIR=${DESTDIR} ARCH=${ARCH}

install: string.m gen.m

