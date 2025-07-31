OBJECTS=crypt.o crypt2.o cryptutil.o blowfish.o bcrypt.o arc4random.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}
	@/bin/echo "done."

%.o: ${SRCDIR}/lib/libc/crypt/%.c
	@/bin/echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I${SRCDIR}/sys -I../include -o $@ $<

