.SUFFIXES:
.DEFAULT:

all: string.d gen.d net.d

%.d: ${ARCH}/%
	@${MAKE} -C $< CFLAGS="${CFLAGS}" SRCDIR=${SRCDIR} ARCH=${ARCH}
