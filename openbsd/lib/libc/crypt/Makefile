OBJECTS=crypt.o crypt2.o cryptutil.o blowfish.o bcrypt.o arc4random.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/crypt/%.c
	gcc -c ${CFLAGS} -I${SRCDIR}/sys -I../include -o $@ $<

