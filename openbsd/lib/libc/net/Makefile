OBJECTS= send.o
#OBJECTS+= recv.o	# presently in libsocket

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/net/%.c
	gcc -c ${CFLAGS} -o $@ $<

