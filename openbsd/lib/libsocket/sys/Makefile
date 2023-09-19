OBJECTS=socket.o connect.o accept.o recvfrom.o sendto.o sendmsg.o recvmsg.o \
	socketpair.o shutdown.o getsockopt.o setsockopt.o bind.o listen.o \
	getpeername.o getsockname.o

SHOBJECTS=socket.so connect.so accept.so recvfrom.so sendto.so sendmsg.so recvmsg.so \
	socketpair.so shutdown.so getsockopt.so setsockopt.so bind.so listen.so \
	getpeername.so getsockname.so

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS} ${SHOBJECTS}

%.o: ../../libc/sys/%.S
	gcc -c ${CFLAGS} -I../../libc/arch/i386 -DNOPIE -o $@ $<

%.so: ../../libc/sys/%.S
	gcc -c ${CFLAGS} -I../../libc/arch/i386 -o $@ $<

