OBJECTS=hash.o hash_bigkey.o hash_buf.o hash_func.o hash_log2.o \
	hash_page.o ndbm.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/db/hash/%.c
	gcc -c ${CFLAGS} -o $@ $<

