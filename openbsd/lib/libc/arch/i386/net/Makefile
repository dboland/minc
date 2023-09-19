OBJECTS=htonl.o htons.o ntohl.o ntohs.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/arch/i386/net/%.S
	gcc -c ${CFLAGS} -o $@ $<

