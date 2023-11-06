.SUFFIXES:
.DEFAULT:

all: string.d gen.d net.d sys.d

%.d: ${ARCH}/%
	@${MAKE} -C $< CFLAGS="${CFLAGS}" SRCDIR=${SRCDIR} ARCH=${ARCH}
