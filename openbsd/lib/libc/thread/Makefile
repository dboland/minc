OBJECTS=unithread_malloc_lock.o unithread_mutex.o unithread_tag.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/thread/%.c
	gcc -c ${CFLAGS} -o $@ $<

