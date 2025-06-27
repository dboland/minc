OBJECTS=htonl.o htons.o ntohl.o ntohs.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}
	@/bin/echo "done."

%.o: ${SRCDIR}/lib/libc/arch/${ARCH}/net/%.S
	@/bin/echo -n "$@ "
	@gcc -c ${CFLAGS} -o $@ $<

