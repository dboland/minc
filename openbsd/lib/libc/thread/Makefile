OBJECTS=unithread_malloc_lock.o unithread_mutex.o unithread_tag.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/thread/%.c
	${XGCC} -c ${CFLAGS} -I../include -o $@ $<

