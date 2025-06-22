OBJECTS=unithread_malloc_lock.o unithread_mutex.o unithread_tag.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/thread/%.c
	gcc -c ${CFLAGS} -I${SRCDIR}/lib/libc/include -o $@ $<

