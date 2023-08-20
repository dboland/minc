OBJECTS=fork.o syscall.o tls.o

all: ${OBJECTS}

%.o: ${PROCESSOR_ARCHITECTURE}/%.S
	gcc -c ${CFLAGS} -o $@ $<

