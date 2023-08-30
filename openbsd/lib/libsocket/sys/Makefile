OBJECTS=socket.o connect.o accept.o recvfrom.o sendto.o sendmsg.o recvmsg.o \
	socketpair.o shutdown.o getsockopt.o setsockopt.o bind.o listen.o \
	getpeername.o getsockname.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ../../libc/sys/%.S
	gcc -c ${CFLAGS} -o $@ $<

